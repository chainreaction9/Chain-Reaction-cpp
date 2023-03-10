#include "PrecompiledHeader.h" //precompiled header
#include "MainGame.h"

//****************************** Static variable initialization ********************************

const std::unordered_map<std::string, glm::vec4> MainGame::_colorMap = MainGame::_createColorMap();

//***********************************************************************************************
//*************************************** Public methods ****************************************

bool MainGame::drawBoard(unsigned long deltaTime) {
	bool gameHasEnded = false; //Boolean flag to detect the end of a game.
	bool empty_bomb_list = this->_currentBombs.empty(); //Check if the board contains any explosive or not.
	if (this->isBlastAnimationRunning() && empty_bomb_list) { //A full sequence of blast is completed as blast animation was running and right now there are no more bombs left.
		this->_blastAnimationRunning = false; //Reset the boolean flag for blast animation runtime.
		this->_eliminatePlayers(true); //Update the list of eliminated players after a full sequence of blasts.
	}
	if (!empty_bomb_list) {
		if (!this->isBlastAnimationRunning()) this->_blastAnimationRunning = true;
		gameHasEnded = this->_runBlastAnimation(deltaTime);
	}
	else {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear color and depth buffer before rendering new frame.
		this->_drawGrid();
		double angle = remainder(this->_angleOfRotation, 360.0);
		for (const auto& boardEntry : this->_BOARD) {
			const auto& key = boardEntry.second.boardCoordinate;
			glm::vec3 center = { (key.first + 0.5f) * this->_CUBE_WIDTH + this->_lowerleft[0],
								(key.second + 0.5f) * this->_CUBE_WIDTH + this->_lowerleft[1],
								0.0f };
			this->_drawOrb(center, boardEntry.second.rotationAxes, angle, boardEntry.second.level, boardEntry.second.color.c_str());
		}
		if (!(this->_BOARD.empty())) this->_angleOfRotation += ((this->_ROTATION_SPEED * deltaTime) / 1000.0);
	}
	if (gameHasEnded) this->resetGameVariables();
	return gameHasEnded;
}
glm::vec2 MainGame::getBoardCoordinates(double mouseX, double mouseY) {
	glm::vec2 output = { -1, -1 };
	glm::vec3 worldCoordinates = this->_getWorldCoordinates(mouseX, mouseY);
	if ((fabs(worldCoordinates[0]) > 0.5 * (this->_COL_DIV) * (this->_CUBE_WIDTH)) || (fabs(worldCoordinates[1]) > 0.5 * (this->_ROW_DIV) * (this->_CUBE_WIDTH))) {
		return output; //The input falls outside the grid. Return negative output.
	}
	int boxx = (int)((worldCoordinates[0] - (this->_lowerleft[0])) / (this->_CUBE_WIDTH));
	int boxy = (int)((worldCoordinates[1] - (this->_lowerleft[1])) / (this->_CUBE_WIDTH));
	output.x = boxx;
	output.y = boxy;
	return output;
}
bool MainGame::isEliminated(std::string player) const {
	bool found = std::find(this->_eliminated.begin(), this->_eliminated.end(), player) == this->_eliminated.end() ? false : true;
	return found;
}
bool MainGame::isExplosive(pair<uint32_t, uint32_t> boardCoordinate) const {
	bool isBomb = false;
	for (const auto& value : this->_currentBombs) {
		if (value.second.boardCoordinate.first == boardCoordinate.first && value.second.boardCoordinate.second == boardCoordinate.second) {
			isBomb = true;
			break;
		}
	}
	return isBomb;
}
MainGame::MainGame(const char* soundSourceName, const char* gridShaderVertexSource, const char* gridShaderFragmentSource, const char* orbShaderVertexSource, const char* orbShaderFragmentSource) {
	if (gridShaderVertexSource) this->_gridShaderVertexSource = gridShaderVertexSource;
	if (gridShaderFragmentSource) this->_gridShaderFragmentSource = gridShaderFragmentSource;
	if (orbShaderVertexSource) this->_orbShaderVertexSource = orbShaderVertexSource;
	if (orbShaderFragmentSource) this->_orbShaderFragmentSource = orbShaderFragmentSource;
	this->_initDefaults();
	if (!this->_initSound(soundSourceName)) return;
}
MainGame::MainGame(const void *soundSource, size_t size, const char* gridShaderVertexSource, const char* gridShaderFragmentSource, const char* orbShaderVertexSource, const char* orbShaderFragmentSource){
	if (gridShaderVertexSource) this->_gridShaderVertexSource = gridShaderVertexSource;
	if (gridShaderFragmentSource) this->_gridShaderFragmentSource = gridShaderFragmentSource;
	if (orbShaderVertexSource) this->_orbShaderVertexSource = orbShaderVertexSource;
	if (orbShaderFragmentSource) this->_orbShaderFragmentSource = orbShaderFragmentSource;
	this->_initDefaults();
	if (!this->_initSound(soundSource, size)) return;
}
MainGame::~MainGame(void) {
	this->resetGameVariables();
	for (auto entry : this->_GRID_VAO_ID) {
		delete entry.second;
	}
	this->_GRID_VAO_ID.clear();
}
bool MainGame::processPlayerInput(uint32_t boardCoordinateX, uint32_t boardCoordinateY) {
	if (this->isBlastAnimationRunning()) return false; //Current board has bombs which needs to be taken care of before processing any input.
	const auto& key = (GameUtilities::BoardKey)this->_boardKey(boardCoordinateX, boardCoordinateY);
	if (!(this->_BOARD.count(key))) {
		GameUtilities::BoardValue value;
		value.rotationAxes.x = Random::random(-1, 1); value.rotationAxes.y = Random::random(-3, 3); value.rotationAxes.z = Random::random(-4, 4);
		if ((value.rotationAxes.x == 0) && (value.rotationAxes.y == 0) && (value.rotationAxes.z == 0)) {
			value.rotationAxes.x += Random::random(0, 1);
			value.rotationAxes.y += Random::random(0, 1);
			value.rotationAxes.z += Random::random(0, 1);
		}
		this->_undoBoard = std::unordered_map<GameUtilities::BoardKey, GameUtilities::BoardValue>(this->_BOARD.begin(), this->_BOARD.end());
		this->_undoTurn = this->_turn;
		this->_undoEliminated = std::vector<std::string>(this->_eliminated.begin(), this->_eliminated.end());

		value.color = this->_players[this->_turn % (int)(this->_players.size())];
		value.level = 1;
		value.boardCoordinate = std::make_pair(boardCoordinateX, boardCoordinateY);
		this->_BOARD[key] = value;
	}
	else {
		if ((this->_players[this->_turn % (int)(this->_players.size())]) != this->_BOARD[key].color) return false;
		this->_undoBoard = std::unordered_map<GameUtilities::BoardKey, GameUtilities::BoardValue>(this->_BOARD.begin(), this->_BOARD.end());
		this->_undoTurn = this->_turn;
		this->_undoEliminated = std::vector<std::string>(this->_eliminated.begin(), this->_eliminated.end());
		this->_BOARD[key].level += 1;
		this->_currentBombs = this->_getBombs(this->_BOARD);
		this->_currentAllNeighbours = this->_getBombNeighbours();
	}
	if (this->_currentBombs.empty()) this->_eliminatePlayers(true);
	return true;
}
void MainGame::resetGameVariables(void) {
	this->_BOARD.clear();
	this->_undoBoard.clear();
	this->_eliminated.clear();
	this->_currentBombs.clear();
	this->_currentAllNeighbours.clear();
	this->_undoEliminated.clear();
	this->_turn = 0;
	this->_undoTurn = 0;
	this->_angleOfRotation = 0.0;
	this->_blastAnimationRunning = false;
	this->_blastDisplacement = 0.0;
}
void MainGame::setAttribute(unsigned int numberOfRows, unsigned int numberOfColumns, const std::vector<std::string>& playerList) {
	this->_ROW_DIV = numberOfRows;
	this->_COL_DIV = numberOfColumns;

	this->_players.clear();
	for (const auto& p : playerList) {
		wxString lowercaseWString = wxString(p.c_str()).Lower();
		this->_players.push_back(std::string(lowercaseWString));
	}

	this->_lowerleft[0] = -0.5 * (this->_COL_DIV) * (this->_CUBE_WIDTH);
	this->_lowerleft[1] = -0.5 * (this->_ROW_DIV) * (this->_CUBE_WIDTH);

	const auto& key = (GameUtilities::BoardKey)this->_boardKey(this->_ROW_DIV, this->_COL_DIV);
	if (!this->_GRID_VAO_ID.count(key)) {
		VertexArrayObject* va = new VertexArrayObject();
		this->_createGridData(this->_ROW_DIV, this->_COL_DIV, va, &this->_GRID_VBO_ID[this->_ROW_DIV - 6][this->_COL_DIV - 6], &this->_GRID_IBO_ID[this->_ROW_DIV - 6][this->_COL_DIV - 6], this->_CUBE_WIDTH);
		this->_GRID_VAO_ID[key] = va;
	}
}
void MainGame::setCanvasSize(unsigned int width, unsigned int height) {
	this->_DISPLAY[0] = width;
	this->_DISPLAY[1] = height;
}
void MainGame::setFrame(AppGUIFrame* frame){
	this->_mainframe = frame;
}
void MainGame::setupCamera() {
	glViewport(0, 0, this->_DISPLAY[0], this->_DISPLAY[1]);
	double cameraDistance = glm::length(this->_cameraTarget - this->_cameraPosition); //Distance between camera and target
	double focusHeight = 0.5 * (this->_ROW_DIV) * (this->_CUBE_WIDTH);
	double calibratedDistance = cameraDistance - this->_CUBE_WIDTH; //Zoom out the camera to avoid rendering near the screen edges.
	glm::mat4 projection = glm::perspective(2 * atan(focusHeight / calibratedDistance), (double)this->_DISPLAY[0] / this->_DISPLAY[1], 1.0, cameraDistance + 100.0);
	glm::mat4 view = glm::lookAt(this->_cameraPosition, this->_cameraTarget, this->_cameraUp);
	glm::mat4 final_mat = projection * view;
	glm::mat4 _model = glm::mat4(1.0f);

	this->_projection = projection;
	this->_modelview = view;

	GLint location = _gridShaderProgram.getUniformLocation("transform");
	this->_gridShaderProgram.use();
	if (location != -1) glUniformMatrix4fv(location, 1, GL_FALSE, &final_mat[0][0]);
	this->_gridShaderProgram.unuse();

	this->_orbShaderProgram.use();
	location = _orbShaderProgram.getUniformLocation("projectionView");
	if (location != -1) glUniformMatrix4fv(location, 1, GL_FALSE, &final_mat[0][0]);
	location = _orbShaderProgram.getUniformLocation("modelTransform");
	if (location != -1) glUniformMatrix4fv(location, 1, GL_FALSE, &_model[0][0]);
	this->_orbShaderProgram.unuse();
}
void MainGame::undo(void) {
	if (this->isBlastAnimationRunning()) return; //safeguard against undoing when a blast animation is ongoing.
	this->_BOARD = std::unordered_map<GameUtilities::BoardKey, GameUtilities::BoardValue>(this->_undoBoard.begin(), this->_undoBoard.end());
	this->_turn = this->_undoTurn;
	this->updateTurn();
	this->_eliminated = std::vector<std::string>(this->_undoEliminated.begin(), this->_undoEliminated.end());
}
void MainGame::updateTurn(void){
	unsigned int playerNumber = 1 + (this->_turn % this->_players.size());
	glm::vec4 playerColor = this->_colorMap.at(this->_players[playerNumber - 1]);
	this->_setColorOfBacksideGrid(0.5f * playerColor.r, 0.5f * playerColor.g, 0.5f * playerColor.b);
	this->_setColorOfFrontsideGrid(playerColor.r, playerColor.g, playerColor.b);
	if (this->_mainframe != nullptr) this->_mainframe->UpdateTurnInGUI(playerNumber);
}
//************************************************************************************************
//******************************************* Private methods ************************************

void MainGame::_applyGridShaderSettings(void) {
	//***************************************************************************
	//**************************************  Grid-data setup *******************
	VertexArrayObject* vao = new VertexArrayObject();
	this->_createGridData(this->_ROW_DIV, this->_COL_DIV, vao, &this->_GRID_VBO_ID[this->_ROW_DIV - 6][this->_COL_DIV - 6], &this->_GRID_IBO_ID[this->_ROW_DIV - 6][this->_COL_DIV - 6], this->_CUBE_WIDTH);
	const auto& key = (GameUtilities::BoardKey)this->_boardKey(this->_ROW_DIV, this->_COL_DIV);
	this->_GRID_VAO_ID[key] = vao;
	this->_gridShaderProgram.compileShaders(this->_gridShaderVertexSource.c_str(), this->_gridShaderFragmentSource.c_str(), true);
	glm::mat4 defaultMatrix = glm::mat4(1.0f);
	this->_gridShaderProgram.use();
	GLint location = this->_gridShaderProgram.getUniformLocation("transform");
	if (location != -1) glUniformMatrix4fv(location, 1, GL_FALSE, &defaultMatrix[0][0]);
	location = this->_gridShaderProgram.getUniformLocation("backsideColor");
	if (location != -1) glUniform4f(location, 0.0f, 0.5f, 0.0f, 1.0f);
	location = this->_gridShaderProgram.getUniformLocation("frontsideColor");
	if (location != -1) glUniform4f(location, 0.0f, 1.0f, 0.0f, 1.0f);
	this->_gridShaderProgram.unuse();
}
void MainGame::_applyOrbShaderSettings(void) {
	//******************************************************************************************************
	//****************************  Particle-data setup *****************************************************
	this->_orbShaderProgram.compileShaders(this->_orbShaderVertexSource.c_str(), this->_orbShaderFragmentSource.c_str(), true);
	this->_orbShaderProgram.use();
	//Update orbColor uniform variable
	GLint location = this->_orbShaderProgram.getUniformLocation("orbColor");
	GLfloat defaultOrbColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	if (location != -1) glUniform4fv(location, 1, defaultOrbColor);
	//***********************************************************
	//Update uniform variable (modelview matrix) in shader
	glm::mat4 defaultMatrix = glm::mat4(1.0f);
	location = this->_orbShaderProgram.getUniformLocation("modelTransform");
	if (location != -1) glUniformMatrix4fv(location, 1, GL_FALSE, &defaultMatrix[0][0]);
	//********************************************************************************
	//Update uniform variable (projection matrix) in shader
	location = this->_orbShaderProgram.getUniformLocation("projectionView");
	if (location != -1) glUniformMatrix4fv(location, 1, GL_FALSE, &defaultMatrix[0][0]);
	//******************************************************************************
	//Update Lightdirection calculated from latitude and longitude in OpenGL coordinate system
	glm::vec3 lightDir = this->getGlCompatibleCoordinate(1.0f, this->_lightDirLatitude, this->_lightDirLongitude);
	location = this->_orbShaderProgram.getUniformLocation("lightDirection");
	if (location != -1) glUniform3f(location, lightDir.x, lightDir.y, lightDir.z);
	//*************************************************************************
	//Update camera position 
	location = this->_orbShaderProgram.getUniformLocation("cameraPosition");
	if (location != -1) glUniform3f(location, this->_cameraPosition.x, this->_cameraPosition.y, this->_cameraPosition.z); //Update uniform variable (camera position) in shader
	//*************************************************************
	//Update uniform variable (vDiffuse) in shader
	location = this->_orbShaderProgram.getUniformLocation("vDiffuse");
	if (location != -1) glUniform3f(location, 0.0f, 0.0f, 0.0f);
	//********************************************************************
	//Update uniform variable (vSpecular) in shader
	location = this->_orbShaderProgram.getUniformLocation("vSpecular");
	if (location != -1) glUniform3f(location, 1.0f, 1.0f, 1.0f);
	//************************************************************************
	//Update uniform variable (vSpecularExponent) in shader
	location = this->_orbShaderProgram.getUniformLocation("vSpecularExponent");
	if (location != -1) glUniform1f(location, 233.3333f);
	//******************************************************************************
	this->_orbShaderProgram.unuse();
	//*************************************************************
}
std::unordered_map<std::string, glm::vec4> MainGame::_createColorMap() {
	std::unordered_map<std::string, glm::vec4> colorMap;
	colorMap["green 1"] = { 0.0f, 1.0f, 127.0f / 255.0f, 1.0f };
	colorMap["grey"] = { 170.0f / 255.0f, 170.0f / 255.0f, 170.0f / 255.0f, 1.0f };
	colorMap["violet"] = { 110.0f / 255.0f, 100.0f / 255.0f, 1.0f, 1.0f };
	colorMap["green 2"] = { 115.0f / 255.0f, 200.0f / 255.0f, 115.0f / 255.0f, 1.0f };
	colorMap["brown"] = { 180.0f / 255.0f, 80.0f / 255.0f, 50.0f / 255.0f, 1.0f };
	colorMap["orange"] = { 1.0f, 0.27f, 0.0f, 1.0f };
	colorMap["pink"] = { 1.0f, 0.412f, 0.706f, 1.0f };
	colorMap["red"] = { 1.0f, 0.0f, 0.0f, 1.0f };
	colorMap["blue"] = { 0.1f, 0.3f, 11.0f, 1.0f };
	colorMap["green"] = { 0.0f, 1.0f, 0.0f, 1.0f };
	colorMap["yellow"] = { 1.0f, 1.0f, 0.0f, 1.0f };
	colorMap["cyan"] = { 0.0f, 1.0f, 1.0f, 1.0f };
	colorMap["purple"] = { 148.0f / 255.0f, 0.0f, 211.0f / 255.0f, 1.0f };
	colorMap["maroon"] = { 0.70f, 0.18f, 0.36f, 1.0f };
	colorMap["white"] = { 1.0f, 1.0f, 1.0f, 1.0f };
	return colorMap;
}
void MainGame::_createGridData(unsigned int numberOfRows, unsigned int numberOfColumns, VertexArrayObject* vao, VertexBufferObject* vbo, IndexBufferObject* ibo, unsigned int cubeWidth, unsigned int centerX, unsigned int centerY) {
	unsigned int width = numberOfColumns * cubeWidth;
	unsigned int height = numberOfRows * cubeWidth;
	VertexStructure::Vertex v;
	std::vector<VertexStructure::Vertex > vertex_list, front_face; //first stores all vertices of back face and then stores vertices of front face;
	for (int i = 0; i != numberOfRows + 1; i++) {
		for (int j = 0; j != numberOfColumns + 1; j++) {
			v.position.x = (float)(centerX - 0.5 * width) + j * cubeWidth;
			v.position.y = (float)(centerY + 0.5 * height) - i * cubeWidth;
			v.position.z = (float)-0.5f * cubeWidth;
			v.color = { 255, 255, 255, 255 };
			vertex_list.push_back(v);
			v.position.z = 0.5f * cubeWidth;
			v.color = { 0, 0, 0, 0 };
			front_face.push_back(v);
		}
	}
	vertex_list.insert(vertex_list.end(), front_face.begin(), front_face.end()); //append the front face vertices to the list.

	vbo->loadNewBuffer(&vertex_list[0], vertex_list.size() * sizeof(VertexStructure::Vertex));
	VertexLayout layout;
	layout.addLayout(GL_FLOAT, 3, GL_FALSE);
	layout.addLayout(GL_FLOAT, 3, GL_FALSE);
	layout.addLayout(GL_FLOAT, 2, GL_FALSE);
	layout.addLayout(GL_UNSIGNED_BYTE, 4, GL_TRUE);
	vao->setVertexBuffer(*vbo, layout);

	std::vector<GLuint> indices;

	for (int i = 0; i != numberOfRows + 1; i++) {
		indices.push_back(i * (numberOfColumns + 1));
		indices.push_back(i * (numberOfColumns + 1) + numberOfColumns);
		indices.push_back((numberOfColumns + 1) * (numberOfRows + 1) + i * (numberOfColumns + 1));
		indices.push_back((numberOfColumns + 1) * (numberOfRows + 1) + i * (numberOfColumns + 1) + numberOfColumns);
	}
	for (int i = 0; i != numberOfColumns + 1; i++) {
		indices.push_back(i);
		indices.push_back(i + numberOfRows * (numberOfColumns + 1));
		indices.push_back((numberOfColumns + 1) * (numberOfRows + 1) + i);
		indices.push_back((numberOfColumns + 1) * (numberOfRows + 1) + i + numberOfRows * (numberOfColumns + 1));
	}
	for (int i = 0; i != (numberOfColumns + 1) * (numberOfRows + 1); i++) {
		indices.push_back(i);
		indices.push_back((numberOfColumns + 1) * (numberOfRows + 1) + i);
	}
	ibo->loadNewBuffer(&indices[0], indices.size());
	vao->setIndexBuffer(*ibo);
}
void MainGame::_drawGrid(){
	this->_gridShaderProgram.use();
	const auto& key = (GameUtilities::BoardKey)this->_boardKey(this->_ROW_DIV, this->_COL_DIV);
	VertexArrayObject* vao = this->_GRID_VAO_ID[key];
	vao->bind();
	for (GLuint i = 0; i < vao->numAttr(); i++) {
		glEnableVertexAttribArray(i);
	}
	glDrawElements(GL_LINES, vao->numIndices(), GL_UNSIGNED_INT, nullptr);
	for (GLuint i = 0; i < vao->numAttr(); i++) {
		glDisableVertexAttribArray(i);
	}
	vao->unbind();
	this->_gridShaderProgram.unuse();
}
void MainGame::_drawOrb(glm::vec3 center, glm::vec3 axes, double angleOfRotation, unsigned int level, const char* colorName) {
	this->_orbShaderProgram.use();
	glm::mat4 _model = glm::mat4(1.0f);
	if (this->_sphere.getColor() != colorName) {
		this->_sphere.setColor(colorName);
		const auto& color = MainGame::_colorMap.at(colorName);
		GLint location = this->_orbShaderProgram.getUniformLocation("orbColor");
		glUniform4f(location, color.r, color.g, color.b, 1.0f);
	}
	if (level == 1) {
		_model = glm::translate(glm::mat4(1.0f), glm::vec3(center.x + Random::random(0, 1) / 3.0, center.y + Random::random(0, 1) / 3.0, center.z + Random::random(0, 1) / 3.0));
		GLint location = this->_orbShaderProgram.getUniformLocation("modelTransform");
		glUniformMatrix4fv(location, 1, GL_FALSE, &_model[0][0]);
		this->_sphere.draw(1);
	}
	else {
		_model = glm::translate(glm::mat4(1.0f), center) * glm::rotate(glm::mat4(1.0f), (float)angleOfRotation, axes);
		GLint location = this->_orbShaderProgram.getUniformLocation("modelTransform");
		glUniformMatrix4fv(location, 1, GL_FALSE, &_model[0][0]);
		this->_sphere.draw(level);
	}
	this->_orbShaderProgram.unuse();
}
void MainGame::_eliminatePlayers(bool updateTurnVariable) {
	if ((this->_turn) < (unsigned int)(this->_players.size())) {
		this->_turn++;
		this->updateTurn();
		return;
	}
	bool valid = false;
	for (const auto& player : this->_players) {
		if (!(this->isEliminated(player))) {
			valid = true;
			for (const auto& boardEntry : this->_BOARD) {
				if (player == boardEntry.second.color) {
					valid = false;
					break;
				}
			}
			if (valid) this->_eliminated.push_back(player);
		}
	}
	if (updateTurnVariable) {
		while (1) {
			this->_turn += 1;
			const std::string& tempPlayer = this->_players[this->_turn % this->_players.size()];
			if (!(this->isEliminated(tempPlayer))) break;
		}
		this->updateTurn();
	}
}
std::vector<std::pair<GameUtilities::BoardKey, GameUtilities::BoardValue>> MainGame::_getBombs(const std::unordered_map<GameUtilities::BoardKey, GameUtilities::BoardValue>& board) {
	std::vector<std::pair<GameUtilities::BoardKey, GameUtilities::BoardValue>> bombList;
	for (const auto& boardEntry : board) {
		const auto& boardCoordinate = boardEntry.second.boardCoordinate;
		unsigned int neighbourCount = 0;
		if (boardCoordinate.first + 1 != this->_COL_DIV) neighbourCount++;
		if (boardCoordinate.first >= 1) neighbourCount++;
		if (boardCoordinate.second + 1 != this->_ROW_DIV) neighbourCount++;
		if (boardCoordinate.second >= 1) neighbourCount++;
		if (neighbourCount <= boardEntry.second.level) bombList.push_back(boardEntry);
	}
	return bombList;
}
std::unordered_map<GameUtilities::BoardKey, uint32_t> MainGame::_getBombNeighbours() {
	std::unordered_map<GameUtilities::BoardKey, uint32_t> allNeighbours;
	for (const auto& bomb : this->_currentBombs) {
		const auto& boxCoordinate = bomb.second.boardCoordinate;
		const std::set<std::pair<uint32_t, uint32_t> >& listOfNeighbours = this->_getNeighbours(bomb.second.boardCoordinate.first, bomb.second.boardCoordinate.second);
		for (const auto& neighbour : listOfNeighbours) {
			const auto& key = (GameUtilities::BoardKey)this->_boardKey(neighbour.first, neighbour.second);
			if (!allNeighbours.count(key)) allNeighbours[key] = 1;
			else allNeighbours[key] += 1;
		}
	}
	return allNeighbours;
}
std::set<std::pair<uint32_t, uint32_t> > MainGame::_getNeighbours(uint32_t boardCoordinateX, uint32_t boardCoordinateY){
	std::set<std::pair<uint32_t,uint32_t> > neighbours;
	if (boardCoordinateX + 1 != this->_COL_DIV) neighbours.insert(std::make_pair(boardCoordinateX + 1, boardCoordinateY));
	if (boardCoordinateX >= 1) neighbours.insert(std::make_pair(boardCoordinateX - 1, boardCoordinateY));
	if (boardCoordinateY + 1 != this->_ROW_DIV) neighbours.insert(std::make_pair(boardCoordinateX, boardCoordinateY + 1));
	if (boardCoordinateY >= 1) neighbours.insert(std::make_pair(boardCoordinateX, boardCoordinateY - 1));
	return neighbours;
}
glm::vec3 MainGame::_getWorldCoordinates(double x, double y) {
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glm::vec4 currentViewport = glm::vec4(viewport[0], viewport[1], viewport[2], viewport[3]);
	glm::vec3 objectLocation = glm::vec3(this->_lowerleft[0], this->_lowerleft[1], 0.5 * (this->_CUBE_WIDTH));
	glm::vec3 output = glm::project(objectLocation, this->_modelview, this->_projection, currentViewport);
	glm::vec3 windowLocation = glm::vec3(x, (double)this->_DISPLAY[1] - y, output.z);
	output = glm::unProject(windowLocation, this->_modelview, this->_projection, currentViewport);
	return output;
}
void MainGame::_initDefaults(void) {

	//************************Generic variables definition
	this->_mainframe = nullptr;
	this->_ROW_DIV = 8;
	this->_COL_DIV = 6;
	this->_CUBE_WIDTH = 20;
	this->_ROTATION_SPEED = 2; // degree/sec rotation speed
	this->_players.push_back("red"); this->_players.push_back("blue");
	this->_blastAnimationRunning = false;
	this->_BLAST_TIME = 0.22; //total time to finish one blast in seconds
	this->_blastDisplacement = 0.0;

	this->_lowerleft[0] = -0.5 * (this->_COL_DIV) * (this->_CUBE_WIDTH);
	this->_lowerleft[1] = -0.5 * (this->_ROW_DIV) * (this->_CUBE_WIDTH);

	this->_DISPLAY[0] = 510;
	this->_DISPLAY[1] = 680;
	this->_cameraPosition = { 0.0f, 0.0f, 300.0f };
	this->_cameraTarget = { 0.0f, 0.0f, 0.0f };
	this->_cameraUp = { 0.0f, 1.0f, 0.0f };
	this->_lightDirLatitude = 0.0f;
	this->_lightDirLongitude = 180.0f;
	this->_audioID = 0;
	this->_audioSourceID = 0;
	float radius = (this->_CUBE_WIDTH) / 4.5f;
	this->_sphere.init(0, 0, 0, radius, VertexStructure::Color(255, 255, 255, 255), 50, 50);
	this->_applyOrbShaderSettings();
	this->_applyGridShaderSettings();
	this->setupCamera();
}
bool MainGame::_initSound(const char* soundSourceName) {
	ALuint soundID = SoundSystem::addAudioData(soundSourceName);
	ALuint sourceID = SoundSystem::addAudioSource(1.0f, 1.0f);
	if (!(soundID && sourceID)) return false;
	this->_audioID = soundID;
	this->_audioSourceID = sourceID;
	return true;
}
bool MainGame::_initSound(const void* soundSource, size_t size) {
	ALuint soundID = SoundSystem::addAudioDataFromMemory((const BYTE*)soundSource, size);
	ALuint sourceID = SoundSystem::addAudioSource(1.0f, 1.0f);
	if (!(soundID && sourceID)) return false;
	this->_audioID = soundID;
	this->_audioSourceID = sourceID;
	return true;
}
bool MainGame::_runBlastAnimation(unsigned long deltaTime) {
	bool gameHasEnded = false; //boolean flag to detect the end of a game.
	if (this->_blastDisplacement == 0.0) SoundSystem::play(this->_audioID, this->_audioSourceID); //play game audio at the beginning of each blast animation.
	glm::vec3 displacedCenter = { 0.0f, 0.0f, 0.0f };
	this->_blastDisplacement += ((this->_CUBE_WIDTH / this->_BLAST_TIME) * deltaTime) / 1000.0;
	if (this->_blastDisplacement < this->_CUBE_WIDTH) { //A blast animation is still running.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear color and depth buffer before rendering new frame.
		this->_drawGrid();
		for (const auto& boardEntry : this->_BOARD) {
			const auto& key = boardEntry.second.boardCoordinate;
			glm::vec3 center = { (key.first + 0.5f) * this->_CUBE_WIDTH + this->_lowerleft[0],
								(key.second + 0.5f) * this->_CUBE_WIDTH + this->_lowerleft[1],
								0.0f };
			const char* color = boardEntry.second.color.c_str();
			double angle = remainder(this->_angleOfRotation, 360.0);
			if (!(this->isExplosive(key))) this->_drawOrb(center, boardEntry.second.rotationAxes, angle, boardEntry.second.level, color);
			else {
				if (this->_currentAllNeighbours.count((GameUtilities::BoardKey)this->_boardKey(key.first + 1, key.second))) {
					displacedCenter.x = center.x + this->_blastDisplacement; displacedCenter.y = center.y; displacedCenter.z = center.z;
					this->_drawOrb(displacedCenter, boardEntry.second.rotationAxes, angle, 1, color);
				}
				if (this->_currentAllNeighbours.count((GameUtilities::BoardKey)this->_boardKey(key.first - 1, key.second))) {
					displacedCenter.x = center.x - this->_blastDisplacement; displacedCenter.y = center.y; displacedCenter.z = center.z;
					this->_drawOrb(displacedCenter, boardEntry.second.rotationAxes, angle, 1, color);
				}
				if (this->_currentAllNeighbours.count((GameUtilities::BoardKey)this->_boardKey(key.first, key.second + 1))) {
					displacedCenter.x = center.x; displacedCenter.y = center.y + this->_blastDisplacement; displacedCenter.z = center.z;
					this->_drawOrb(displacedCenter, boardEntry.second.rotationAxes, angle, 1, color);
				}
				if (this->_currentAllNeighbours.count((GameUtilities::BoardKey)this->_boardKey(key.first, key.second - 1))) {
					displacedCenter.x = center.x; displacedCenter.y = center.y - this->_blastDisplacement; displacedCenter.z = center.z;
					this->_drawOrb(displacedCenter, boardEntry.second.rotationAxes, angle, 1, color);
				}
			}
		}
		this->_angleOfRotation += ((this->_ROTATION_SPEED * deltaTime) / 1000.0);
	}
	else { //A blast animation is completed. Update board.
		const auto& bomb = this->_currentBombs[0];
		GameUtilities::BoardKey key = bomb.first;
		std::string bombColorName = bomb.second.color;
		for (const auto& bombEntry : this->_currentBombs) this->_BOARD.erase(bombEntry.first);
		for (const auto& neighbourEntry : this->_currentAllNeighbours) {
			key = neighbourEntry.first;
			if (this->_BOARD.count(key)) {
				this->_BOARD[key].level += neighbourEntry.second;
				this->_BOARD[key].color.assign(bombColorName);
			}
			else {
				GameUtilities::BoardValue value;
				value.rotationAxes.x = Random::random(-1, 1); value.rotationAxes.y = Random::random(-3, 3); value.rotationAxes.z = Random::random(-4, 4);
				if ((value.rotationAxes.x == 0) && (value.rotationAxes.y == 0) && (value.rotationAxes.z == 0)) {
					value.rotationAxes.x += Random::random(0, 1);
					value.rotationAxes.y += Random::random(0, 1);
					value.rotationAxes.z += Random::random(0, 1);
				}
				value.level = neighbourEntry.second;
				value.color = bombColorName;
				value.boardCoordinate = this->_inverseKey(key);
				this->_BOARD[key] = value;
			}
		}
		//Update the current list of explosive sites. If new explosive sites are created after the preceeding blasts, those are processed in the next animation.
		this->_currentBombs = this->_getBombs(this->_BOARD);
		//Update the current list of sites that have an explosive neighbouring site.
		this->_currentAllNeighbours = this->_getBombNeighbours();
		//Reset the variable for processing the next blast animation.
		this->_blastDisplacement = 0.0;
		//Update the list of eliminated players after a blast. This does not alter the turn variable of the game.
		this->_eliminatePlayers(false);
		//Check if game has ended after a blast:
		if ((int)this->_eliminated.size() == (int)this->_players.size() - 1) {
			AppGUIFrame* frame = this->getFrame();
			if (frame != nullptr) frame->swapBuffers();
			std::string winnerName = "";
			int winnderIndex = 1;
			for (const auto& player : this->_players) {
				if (!(this->isEliminated(player))) {
					winnerName = player;
					break;
				}
				winnderIndex++;
			}
			wxMessageBox(wxString::Format("Player %d (%s) won! Game Over!", winnderIndex, winnerName.c_str()), wxT("Information"), wxOK | wxICON_EXCLAMATION);
			gameHasEnded = true; //game has ended.
		}
	}
	return gameHasEnded; //game has not yet ended.
}
void MainGame::_setColorOfBacksideGrid(float colorR, float colorG, float colorB) {
	this->_gridShaderProgram.use();
	GLint location = this->_gridShaderProgram.getUniformLocation("backsideColor");
	if (location != -1) glUniform4f(location, colorR, colorG, colorB, 1.0f);
	this->_gridShaderProgram.unuse();
}
void MainGame::_setColorOfFrontsideGrid(float colorR, float colorG, float colorB) {
	this->_gridShaderProgram.use();
	GLint location = this->_gridShaderProgram.getUniformLocation("frontsideColor");
	if (location != -1) glUniform4f(location, colorR, colorG, colorB, 1.0f);
	this->_gridShaderProgram.unuse();
}
