#include "PrecompiledHeader.h"
#include "AppGLCanvas.h"
#include "MainGame.h"

//******************** Public member definitions
AppGLCanvas::AppGLCanvas(wxWindow* parent, int* attribList) : wxGLCanvas(parent, wxID_ANY, attribList, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE) {
	this->SetBackgroundColour(wxColour(0, 0, 0));
	bool success = false;
	this->_context = new AppGLContext(this, success); //Create OpenGL context from wxGLCanvas
	if (!success) {
		delete this->_context;
		this->_context = nullptr;
		wxMessageBox(wxT("Failed to initialize OpenGL context. Your operating system may not support this app."), wxT("OpenGL initialization error."), wxOK | wxICON_ERROR);
		return;
	}
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	this->_initDefaultVariables();
}
AppGLCanvas::~AppGLCanvas() {
	if (this->_context) delete this->_context;
}
void AppGLCanvas::ApplyDefaultCameraSetup(void) {
	wxSize size = this->GetClientSize();
	glViewport(0, 0, size.x, size.y);
	this->_context->SetCurrent(*this);
	//******************* Default projection matrix
	this->defaultProj = glm::perspective((float)glm::radians(45.0f), (float)size.x / size.y, 1.0f, 100.0f);
	//******************* Default camera view matrix
	this->_updateCameraView();
}
void AppGLCanvas::OnIdle(wxIdleEvent& event) {
	unsigned long deltaTime = (this->_mBeginFrame - this->_mLastFrame).ToLong();
	this->_mBeginFrame = wxGetLocalTimeMillis();
	unsigned long desiredFrameTime = (1000.0 / FPS_RATE);
	if (deltaTime < desiredFrameTime) { event.RequestMore(); return; }
	bool taskCompleted = this->_finishIdleTask(deltaTime);
	this->_mLastFrame = this->_mBeginFrame;
	if (!taskCompleted) event.RequestMore();
	else event.Skip();
}
void AppGLCanvas::OnKeyUp(wxKeyEvent& event) {
	GameUtilities::GameState* gameInfo = (GameUtilities::GameState*)this->GetClientData();
	if (!gameInfo) { event.Skip(); return; }
	MainGame* game = (MainGame*)gameInfo->game;
	if (!game) {
		event.Skip();
		return;
	}
	switch (event.GetKeyCode()) {
	case ((int)'U'):
		if (gameInfo->isOnline) {
			event.Skip();
			return;
		}
		game->undo();
		break;
	default:
		return;
	}
}
void AppGLCanvas::OnMouseEvents(wxMouseEvent& event) {
	if (event.Entering() || event.ButtonUp() && this->_mouseCursorHidden) {
		wxCursor myCursor = wxStockCursor(wxCURSOR_DEFAULT);
		SetCursor(myCursor);
		this->_mouseCursorHidden = false;
	}
	GameUtilities::GameState* gameInfo = (GameUtilities::GameState*)this->GetClientData();
	if (event.LeftUp()) { //Handle mouse clicks
		wxPoint position = event.GetPosition();
		this->SetMouseData(position.x, position.y, true);
		event.Skip(); //If game has not started, mouse click has no effect.
		return;
	}
	if (event.Dragging()) { //Handle camera rotation
		unsigned long delta_t = (this->_mBeginFrame - this->_mLastFrame).ToLong();
		wxPoint newPos = event.GetPosition();
		int delta_x = newPos.x - this->_currentMouseX;
		int delta_y = newPos.y - this->_currentMouseY;
		this->_currentMouseX = newPos.x;
		this->_currentMouseY = newPos.y;
		if (delta_x * delta_x + delta_y * delta_y < 10) {
			if (!(gameInfo == nullptr || gameInfo->game_started)) {
				this->_cameraLatitude += delta_t * this->_cameraRotationSpeed * delta_y / 1000.0f;
				this->_cameraLongitude += delta_t * this->_cameraRotationSpeed * delta_x / 1000.0f;
				if (!this->_mouseCursorHidden) {
					wxCursor myCursor = wxStockCursor(wxCURSOR_BLANK);
					SetCursor(myCursor);
					this->_mouseCursorHidden = true;
				}
				this->_updateCameraView();
			}
		}
	}
}
void AppGLCanvas::OnPaint(wxPaintEvent& event) {
	wxPaintDC dc(this);
	this->_context->SetCurrent(*this);
	SwapBuffers();
}
void AppGLCanvas::OnResize(wxSizeEvent& event) {
	GameUtilities::GameState* gameInfo = (GameUtilities::GameState*)this->GetClientData();
	if (gameInfo != nullptr) {
		if (gameInfo->game_started) {
			void *game = nullptr;
			if (!gameInfo->isOnline) {
				MainGame* game = (MainGame*)gameInfo->game;
				wxSize size = event.GetSize();
				game->setCanvasSize(size.x, size.y);
				game->setupCamera();
			}
		}
		else {
			this->ApplyDefaultCameraSetup();
		}
		return;
	}
	event.Skip();
}
bool AppGLCanvas::SetMouseData(int x, int y, bool userInput) {
	GameUtilities::GameState* gameInfo = (GameUtilities::GameState*)this->GetClientData();
	if ((gameInfo != nullptr) && (gameInfo->game_started) && (gameInfo->mouse_data_processed)) {
		this->_mouseX = x;
		this->_mouseY = y;
		gameInfo->mouse_data_processed = false;
		return true;
	}
	return false;
}
void AppGLCanvas::SetResourceData(const ResourceUtilities::ResourceData* source) {
	this->_resourceData = source;
	if (this->_resourceData != nullptr) {
		bool isParticleShaderSourcePresent = false;
		if (this->_resourceData->particleShaderVertexSource) {
			this->_particleShaderVertexSource.assign(this->_resourceData->particleShaderVertexSource);
			isParticleShaderSourcePresent = true;
		}
		if (this->_resourceData->particleShaderFragmentSource) {
			this->_particleShaderFragmentSource.assign(this->_resourceData->particleShaderFragmentSource);
			isParticleShaderSourcePresent = true;
		}
		if (isParticleShaderSourcePresent) this->_applyParticleShaderSettings();
		bool isTextShaderSourcePresent = false;
		if (this->_resourceData->textShaderVertexSource) {
			this->_textShaderVertexSource.assign(this->_resourceData->textShaderVertexSource);
			isTextShaderSourcePresent = true;
		}
		if (this->_resourceData->textShaderFragmentSource) {
			this->_textShaderFragmentSource.assign(this->_resourceData->textShaderFragmentSource);
			isTextShaderSourcePresent = true;
		}
		if (isTextShaderSourcePresent) this->_applyTextShaderSettings();
		if (isParticleShaderSourcePresent || isTextShaderSourcePresent) this->ApplyDefaultCameraSetup();
		this->_textShader.applyMaterial(this->_resourceData->defaultSphereMaterial);
		this->_particleShader.applyMaterial(this->_resourceData->defaultSphereMaterial);
	}
}
AppGLContext::AppGLContext(wxGLCanvas* canvas, bool& success) : wxGLContext(canvas) {
	SetCurrent(*canvas);
	GLenum error = glewInit();
	if (error != GLEW_OK) {
		success = false;
		return;
	}
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	success = true;
}
//******************** Private member definitions
void AppGLCanvas::_applyParticleShaderSettings(void) {
	//*************************************************************
	//************************ Browinian-particle-processing-shader setup
	//Vertex shader for rendering brownian particles
	this->_particleShader.compileShaders(this->_particleShaderVertexSource.c_str(), this->_particleShaderFragmentSource.c_str(), true);
	this->_particleShader.use();
	//Update uniform variable (modelView matrix) in shader
	GLint location = _particleShader.getUniformLocation("modelTransform");
	glm::mat4 _model = glm::mat4(1.0f);
	if (location != -1) glUniformMatrix4fv(location, 1, GL_FALSE, &_model[0][0]);
	//**********************************************
	//Update uniform variable (default color for particles) in shader
	location = this->_particleShader.getUniformLocation("particleColor");
	if (location != -1) glUniform4f(location, 1, 1, 0, 1);
	//**********************************************
	//Update Lightdirection calculated from latitude and longitude in OpenGL coordinate system
	glm::vec3 lightDir = this->getGlCompatibleCoordinate(1.0f, this->_lightDirLatitude, this->_lightDirLongitude); //direction determined by lattitude and longitude
	location = this->_particleShader.getUniformLocation("lightDirection");
	if (location != -1) glUniform3f(location, lightDir.x, lightDir.y, lightDir.z); //Update uniform variable (lightDirection) in shader
	//***********************************************
	//Update camera position 
	glm::vec3 cameraPosition = this->getGlCompatibleCoordinate(this->_cameraDistance, this->_cameraLatitude, this->_cameraLongitude);
	location = this->_particleShader.getUniformLocation("cameraPosition");
	if (location != -1) glUniform3f(location, cameraPosition.x, cameraPosition.y, cameraPosition.z); //Update uniform variable (camera position) in shader
	//*************************************************************
	if (this->_resourceData != nullptr) {
		this->_particleShader.applyMaterial(this->_resourceData->defaultSphereMaterial);
	}
	else {
		location = this->_particleShader.getUniformLocation("vDiffuse");
		if (location != -1) glUniform3f(location, 0.8f, 0.8f, 0.8f); //Update uniform variable (vDiffuse) in shader
		location = this->_particleShader.getUniformLocation("vSpecular");
		if (location != -1) glUniform3f(location, 1.0f, 1.0f, 1.0f); //Update uniform variable (vSpecular) in shader
		location = this->_particleShader.getUniformLocation("vSpecularExponent");
		if (location != -1) glUniform1f(location, 1.0f); //Update uniform variable (vSpecularExponent) in shader
	}
	this->_particleShader.unuse();
	//***************************************************************************************
}
void AppGLCanvas::_applyTextShaderSettings(void) {

	//******************************** Text-processsing-shader setup
	this->_textShader.compileShaders(this->_textShaderVertexSource.c_str(), this->_textShaderFragmentSource.c_str(), true);
	this->_textShader.use();
	//Update uniform variable (modelView matrix) in shader
	GLint location = this->_textShader.getUniformLocation("modelTransform");
	glm::mat4 _model = glm::mat4(1.0f);
	if (location != -1) glUniformMatrix4fv(location, 1, GL_FALSE, &_model[0][0]);
	//**************************************************************************
	//Update uniform variable (angle) in shader
	location = this->_textShader.getUniformLocation("angle");
	if (location != -1) glUniform1f(location, this->_angle);
	//****************************************************************************
	//Update Lightdirection calculated from latitude and longitude in OpenGL coordinate system
	glm::vec3 lightDir = this->getGlCompatibleCoordinate(1.0f, this->_lightDirLatitude, this->_lightDirLongitude); //direction determined by lattitude and longitude
	location = this->_textShader.getUniformLocation("lightDirection");
	if (location != -1) glUniform3f(location, lightDir.x, lightDir.y, lightDir.z); //Update uniform variable (lightDirection) in shader
	//*******************************************************************************
	//Update camera position
	location = this->_textShader.getUniformLocation("cameraPosition");
	glm::vec3 cameraPosition = this->getGlCompatibleCoordinate(this->_cameraDistance, this->_cameraLatitude, this->_cameraLongitude);
	if (location != -1) glUniform3f(location, cameraPosition.x, cameraPosition.y, cameraPosition.z); //Update uniform variable (camera position) in shader
	//*********************************************************************************
	if (this->_resourceData != nullptr) {
		this->_textShader.applyMaterial(this->_resourceData->defaultSphereMaterial);
	}
	else {
		location = this->_textShader.getUniformLocation("vDiffuse");
		if (location != -1) glUniform3f(location, 0.800000f, 0.000000f, 0.002118f); //Update uniform variable (vDiffuse) in shader
		location = this->_textShader.getUniformLocation("vSpecular");
		if (location != -1) glUniform3f(location, 1.0f, 1.0f, 1.0f); //Update uniform variable (vSpecular) in shader
		location = this->_textShader.getUniformLocation("vSpecularExponent");
		if (location != -1) glUniform1f(location, 233.333333f); //Update uniform variable (vSpecularExponent) in shader
	}
	this->_textShader.unuse();
	//*****************************************************************************
}
bool AppGLCanvas::_finishIdleTask(unsigned long deltaTime) {
	GameUtilities::GameState* gameInfo = (GameUtilities::GameState*)this->GetClientData(); //Recover information on the current game state.
	if (gameInfo == nullptr) {
		this->_renderDefaultScene(); //This function renders the default background at the start of the application.
		this->Refresh(true);
		return true;
	}
	if (gameInfo->game_started && !(gameInfo->new_game || gameInfo->go_back_main)) {
		bool completed = false;
		if (!gameInfo->isOnline) {
			//An offline game has started. Run main game loop.
			completed = this->_offlineGameLoop(gameInfo, deltaTime);
		}
		if (completed) this->Refresh(true);
		return completed;
	}
	else if (gameInfo->game_started && gameInfo->new_game) {
		// This option can be triggered only in offline mode.
		MainGame* game = (MainGame*)gameInfo->game;
		game->resetGameVariables();
		gameInfo->new_game = false;
		gameInfo->game_ended = false;
		gameInfo->mouse_data_processed = true;
		game->updateTurn();
		return false;
	}
	else if (gameInfo->game_started && gameInfo->go_back_main != 0) {
		gameInfo->game_started = false;
		gameInfo->game_ended = true;
		gameInfo->new_game = false;
		gameInfo->mouse_data_processed = true;
		if (!gameInfo->isOnline) {
			MainGame* game = (MainGame*)gameInfo->game;
			if (!game) return true;
			game->resetGameVariables();
			AppGUIFrame* frame = game->getFrame();
			if (frame != nullptr) frame->ShowGamePanel(false);
		}
		this->ApplyDefaultCameraSetup();
		gameInfo->go_back_main = 0;
		return false;
	}
	else {
		this->_renderDefaultScene(); //This function renders the default background at the start of the application.
		this->Refresh(true);
		return true;
	}
}
void AppGLCanvas::_initDefaultVariables(void) {
	this->_angle = 0.0f;
	this->_sampleUvSphere.init(0, 0, 0, 1.0f);
	this->_resourceData = nullptr;
	this->_mouseCursorHidden = false;
	this->_mouseX = this->_mouseY = 0;
	this->_mBeginFrame = this->_mLastFrame = wxGetLocalTimeMillis();
	//************************************************* Particle-data setup *******************
	this->_skyboxSize = 30.0f; //size of bounding box
	this->_mNumParticle = 100; //Total number of particles in the bounding box
	for (int i = 0; i < this->_mNumParticle; i++) {
		GameUtilities::Particle entry;
		entry.position = glm::vec3(this->_skyboxSize * Random::random(-1, 1), this->_skyboxSize * Random::random(-1, 1), this->_skyboxSize * Random::random(-1, 1));
		entry.color = glm::vec3(Random::random(0, 1), Random::random(0, 1), Random::random(0, 1));
		entry.level = 1;
		entry.isFlickering = Random::random(0, 1) > 0.75 ? true : false; // 25% chance that a particle is of flickering type.
		this->_particleData.push_back(entry);
	}
	this->_lightDirLatitude = 0.0f;
	this->_lightDirLongitude = 180.0f;
	//******************************** Set default camera properties
	this->_cameraDistance = this->_skyboxSize; //Camera is located at the furthest point of the skybox (which is a sphere).
	this->_cameraLatitude = 0;
	this->_cameraLongitude = 0;
	this->_cameraTarget = glm::vec3(0, 0, 0); // looks at origin which is the center of the spherical skybox.
	this->_cameraUp = glm::vec3(0, 1, 0);
	this->_cameraRotationSpeed = 360.0f;
	//******************************** Apply default shader settings for particle rendering
	this->_applyParticleShaderSettings();
	//********************************************************************************
	//******************************** Apply default shader settings for text rendering
	this->_applyTextShaderSettings();
	//******************************** Light direction specification
	//************* Apply default camera settings
	this->ApplyDefaultCameraSetup();
	//*******************************************************************
}
bool AppGLCanvas::_offlineGameLoop(GameUtilities::GameState* gameInfo, unsigned long deltaTime) {
	MainGame* game = (MainGame*)gameInfo->game;
	if (!gameInfo->game_ended) {
		bool isInputProcessedByGame = false;
		if (!gameInfo->mouse_data_processed) {
			glm::vec2 boardCoordinate = game->getBoardCoordinates(this->_mouseX, this->_mouseY);
			if (boardCoordinate.x >= 0 && boardCoordinate.y >= 0) {
				isInputProcessedByGame = game->processPlayerInput((uint32_t)boardCoordinate.x, (uint32_t)boardCoordinate.y);
			}
			gameInfo->mouse_data_processed = true;
		}
		bool gameHasEnded = game->drawBoard(deltaTime);
		if (gameHasEnded) {
			gameInfo->game_started = false;
			gameInfo->game_ended = true;
			AppGUIFrame* frame = game->getFrame();
			if (frame != nullptr) frame->ShowGamePanel(false);
			this->ApplyDefaultCameraSetup();
			return false;
		}
	}
	return true;
}
void AppGLCanvas::_updateBrownianParticleMotion(void) {
	//***************** Draw brownian particles
	GLint location = this->_particleShader.getUniformLocation("modelTransform");
	GLint colorLocation = this->_particleShader.getUniformLocation("particleColor");
	GLint flickeringLocation = this->_particleShader.getUniformLocation("isFlickering");
	for (int i = 0; i < this->_mNumParticle; i++) {
		glm::mat4 model = glm::translate(glm::mat4(1.0f), this->_particleData[i].position);
		if (location != -1) glUniformMatrix4fv(location, 1, GL_FALSE, &model[0][0]);
		if (flickeringLocation != -1) glUniform1f(flickeringLocation, this->_particleData[i].isFlickering ? 1.0f : 0.0f);
		if (colorLocation != -1) glUniform4f(colorLocation, this->_particleData[i].color.r, this->_particleData[i].color.g, this->_particleData[i].color.b, 1.0f);
		this->_sampleUvSphere.draw(this->_particleData[i].level);
		float radius = this->_sampleUvSphere.getRadius();
		float distance = glm::length(this->_particleData[i].position);
		if (distance < this->_skyboxSize - radius) {
			float dx = 0.2 * Random::random(-1, 1);
			this->_particleData[i].position.x += dx;
			float dy = 0.2 * Random::random(-1, 1);
			this->_particleData[i].position.y += dy;
			float dz = 0.2 * Random::random(-1, 1);
			this->_particleData[i].position.z += dz;
		}
		else {
			this->_particleData[i].position *= 0.9;
		}
	}
	this->_particleShader.unuse();
	///////////////////
}
void AppGLCanvas::_updateCameraView(void) {
	glm::vec3 cameraPosition = this->getGlCompatibleCoordinate(this->_cameraDistance, this->_cameraLatitude, this->_cameraLongitude);
	this->defaultView = glm::lookAt(cameraPosition, this->_cameraTarget, this->_cameraUp);
	glm::mat4 final_mat = this->defaultProj * this->defaultView;
	this->_textShader.use();
	GLint location = this->_textShader.getUniformLocation("projectionView");
	if (location != -1) glUniformMatrix4fv(location, 1, GL_FALSE, &final_mat[0][0]);
	location = this->_textShader.getUniformLocation("cameraPosition");
	if (location != -1) glUniform3f(location, cameraPosition.x, cameraPosition.y, cameraPosition.z); //Update uniform variable (camera position) in shader
	this->_textShader.unuse();
	this->_particleShader.use();
	location = this->_particleShader.getUniformLocation("projectionView");
	if (location != -1) glUniformMatrix4fv(location, 1, GL_FALSE, &final_mat[0][0]);
	location = this->_particleShader.getUniformLocation("cameraPosition");
	if (location != -1) glUniform3f(location, cameraPosition.x, cameraPosition.y, cameraPosition.z); //Update uniform variable (camera position) in shader
	this->_particleShader.unuse();
}
void AppGLCanvas::_renderDefaultScene(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear color and depth buffer before rendering new frame.
	this->_angle = (this->_angle + 1) % 360; //Increase global angle variable
	//*************************** Draw background text

	if (this->_resourceData != nullptr && this->_resourceData->textObject != nullptr) {
		this->_textShader.use();
		GLint location = this->_textShader.getUniformLocation("modelTransform");
		if (location != -1) this->_resourceData->textObject->DrawLine(this->_textShader, location, "WELCOME TO THE WORLD\nOF\nCHAIN REACTION", glm::vec3(0, 0, 0), glm::vec3(0, (float)this->_angle, 0), "center", "center");
		this->_textShader.unuse();
	}//***************************

	//*************************** Draw brownian particles
	this->_particleShader.use();
	GLint location = this->_particleShader.getUniformLocation("angle");
	if (location != -1) glUniform1f(location, this->_angle);
	this->_updateBrownianParticleMotion();
}

//*************************** Declare event table of the AppGLCanvas instance *****************

BEGIN_EVENT_TABLE(AppGLCanvas, wxGLCanvas)
EVT_PAINT(AppGLCanvas::OnPaint)
EVT_IDLE(AppGLCanvas::OnIdle)
EVT_MOUSE_EVENTS(AppGLCanvas::OnMouseEvents)
EVT_KEY_UP(AppGLCanvas::OnKeyUp)
EVT_SIZE(AppGLCanvas::OnResize)
END_EVENT_TABLE()

//***********************************************************************************************