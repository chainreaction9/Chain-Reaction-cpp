#pragma once
#ifndef APP_GL_CANVAS_H
#define APP_GL_CANVAS_H

#include "PrecompiledHeader.h"
#include "Text.h"
#include "Utilities.h"
#include "UvSphere.h"
#include "IcoSphere.h"

/*
* @brief A class to obtain the OpenGL context in a wxGLCanvas. It is derived from the base class wxGLContext.
*/
class AppGLContext : public wxGLContext {
public:
	/**
	* @brief Default constructor of the AppGLContext class which creates OpenGL context for a wxGLCanvas instance.
	* @param {wxGLCanvas*} canvas: pointer to a wxGLCanvas instance.
	* @param[out] {bool&} success: a reference to boolean variable which is set to true on successful creation of an OpenGL context, false otherwise.
	*/
	AppGLContext(wxGLCanvas* canvas, bool& success);
};
/*
* @brief A class representing the canvas instance used for OpenGL rendering. It is derived from the baseclass wxGLCanvas.
*/
class AppGLCanvas : public wxGLCanvas {
public:
	/*
	* @brief Default constructor of the AppGLCanvas class for creating OpenGL canvas instance.
	* @param {wxWindow*} parent: pointer to the parent wxWidget.
	* @param {wxSize*} size: default size of the canvas object.
	* @param {int*} attribList: list of OpenGL attributes which is passed on to the wxGLCanvas constructor.
	*/
	AppGLCanvas(wxWindow* parent, wxSize size = wxSize(1200, 1200), int* attribList = NULL);
	/*
	* @brief Resets the camera configurations (i.e., viewport, camera location, camera direction, lights, projection matrix, etc.)
	* of the openGL context.
	*/
	void ApplyDefaultCameraSetup(void);
	inline bool IsCanvasReadyForRendering(void) const { return (this->_isGlewInitialized && !(this->_isPendingResourceData)); }
	/**
	* @brief Initializes GLEW library.
	* @returns {bool} true if GLEW is successfully initialized, false otherwise.
	*/
	bool InitGLEW();
	/**
	* @brief Initializes OpenGL context on this wxGLCanvas.
	* @returns {bool} true if OpenGL context is successfully created, false otherwise.
	*/
	bool InitOpenGLContext();
	/**
	* @brief Handles IDLE events of AppGLCanvas instance and is responsible for maintaining communication between the main game loop
	* @brief of the application and the GUI events of the canvas instance.
	* @param[out] {wxIdleEvent&} event: a reference to the idle event information provided by the canvas instance.
	*/
	void OnIdle(wxIdleEvent& event);
	/*
	* @brief Handles key up events of AppGLCanvas instance. If there is a game running, updates the game state according to the input key.
	* @param[out] {wxKeyEvent&} event: a reference to the key event information provided by the canvas instance.
	* @see  AppGLCanvas::OnIdle
	*/
	void OnKeyUp(wxKeyEvent& event);
	/*
	* @brief Handles mouse click events. If there is a game running, the mouse input data (position) is registered
	* @brief and passed on to the game object (take a look at `AppGLCanvas::OnIdle` method) for further processing in idle time.
	* @param[out] {wxMouseEvent&} event: a reference to the mouse event information provided by the canvas instance.
	* @see  AppGLCanvas::OnIdle
	* @see AppGLCanvas::SetMouseData
	*/
	void OnMouseEvents(wxMouseEvent& event);
	/*
	* @brief Handles paint event of the AppGLCanvas instance and maintains fixed frame rate.
	* @param[out] {wxPainEvent&} event: a reference to the paint event information provided by the canvas instance.
	*/
	void OnPaint(wxPaintEvent& event);
	void OnResize(wxSizeEvent& event);
	/*
	* @brief Registers the mouse input data (position) for further processing in idle time is eventually passed on to the
	* @brief game object (take a look at `AppGLCanvas::OnIdle` method).
	* @param {int} x: x-coordinate of the mouse pointer in AppGLCanvas (measured w.r.t. the canvas area: topleft of the AppGLCanvas is (0,0)).
	* @param {int} y: y-coordinate of the mouse pointer in AppGLCanvas (measured w.r.t. the canvas area: topleft of the AppGLCanvas is (0,0)).
	* @see  AppGLCanvas::OnIdle
	*/
	bool SetMouseData(int x, int y, bool userInput = true);
	/*
	* @brief Stores the resource data passed by `OnInit` method of the application instance to a member variable for further usage.
	* @param {const ResourceData*} source: pointer to the resource data.
	*/
	void SetResourceData(const ResourceUtilities::ResourceData* source);
	/*
	* @brief Stores the current projection matrix.
	*/
	glm::mat4 defaultProj = glm::mat4(1.0f);
	/*
	* @brief Stores the current view matrix.
	*/
	glm::mat4 defaultView = glm::mat4(1.0f);
	inline glm::vec3 getGlCompatibleCoordinate(float radius, float latitudeDegree, float longitudeDegree) { return glm::vec3({ radius * sin(glm::radians(longitudeDegree)) * cos(glm::radians(latitudeDegree)), radius * sin(glm::radians(latitudeDegree)), radius * cos(glm::radians(longitudeDegree)) * cos(glm::radians(latitudeDegree)) }); }

private:
	int _angle = 0;
	double _contentScalingFactor = 1.0;
	bool _isGlewInitialized = false;
	bool _isPendingResourceData = false;
	bool _mouseCursorHidden = false;
	int _mouseX = 0, _mouseY = 0;
	float _currentMouseX = 0, _currentMouseY = 0;
	float _cameraDistance = 30.0f;
	float _cameraLatitude = 0; //latitude (measured in degree) of the camera location.
	float _cameraLongitude = 0; //longitude (measured in degree) of the camera location.
	glm::vec3 _cameraTarget = glm::vec3(0, 0, 0);
	glm::vec3 _cameraUp = glm::vec3(0, 1, 0);
	float _cameraRotationSpeed = 1.0f; //Speed (degree / mouse_increment / sec) at which camera rotates while dragging mouse pointer.
	GLfloat _lightDirLatitude = -10.0f;
	GLfloat _lightDirLongitude = 0.0f;
	unsigned int _mNumParticle = 0;
	float _skyboxSize = 10.0f;
	UvSphere _sampleUvSphere;
	IcoSphere _sampleIcoSphere;
	wxLongLong _mBeginFrame = 0, _mLastFrame = 0;
	ResourceUtilities::ResourceData _resourceData;
	AppGLContext* _context = nullptr;
	Shaders _textShader, _particleShader;
	std::shared_ptr<Text> _textObject; //Pointer to the text object which is used for rendering 3D text.
	std::unordered_map<std::string, MeshData*> _characterSet; //Map between name of the 26 alphabets and its mesh data. This is required as an argument for creating a Text object.
	std::string _textShaderVertexSource = "#version 300 es\r\n\
		layout(location=0) in vec3 vertexPosition;\r\n\
		layout(location=1) in vec3 vertexNormal;\r\n\
		layout(location=2) in vec2 texCoord;\r\n\
		out vec3 normal;\r\n\
		out vec4 vPosition;\r\n\
		uniform mat4 projectionView;\r\n\
		uniform mat4 modelTransform;\r\n\
		void main(){\r\n\
		    vPosition = modelTransform * vec4(vertexPosition, 1.0);\r\n\
			normal = normalize(vec3(modelTransform * vec4(vertexNormal, 0.0)));\r\n\
			gl_Position = projectionView * vPosition;\r\n\
		}";
	std::string _textShaderFragmentSource = "#version 300 es\r\n\
		precision highp float;\r\n\
		precision highp int;\r\n\
		in vec3 normal;\r\n\
		in vec4 vPosition;\r\n\
		out vec4 outputColor;\r\n\
		uniform vec3 cameraPosition;\r\n\
		uniform vec3 vDiffuse;\r\n\
		uniform vec3 vSpecular;\r\n\
		uniform float vSpecularExponent;\r\n\
		uniform vec3 lightDirection;\r\n\
		void main(){\r\n\
		    vec3 viewDir = normalize(cameraPosition - vPosition.xyz);\r\n\
			vec3 sunDir = normalize(lightDirection);\r\n\
			vec3 reflectionVector =  normalize(2.0 * dot(normal, -sunDir) * normal + sunDir);\r\n\
			float brightness = clamp(dot(normal, -sunDir), 0.0, 1.0);\r\n\
			vec3 modifiedDiffuseColor = vSpecular;\r\n\
			vec3 specularComponent = clamp(vSpecular * pow(dot(reflectionVector, viewDir), vSpecularExponent), 0.0, 1.0);\r\n\
			vec3 color = clamp(modifiedDiffuseColor + specularComponent, 0.0, 1.0);\r\n\
			outputColor = vec4(color * brightness, 1.0);\r\n\
		}";
	std::string _particleShaderVertexSource = "#version 300 es\r\n\
		layout(location = 0) in vec3 vertexPosition;\r\n\
		layout(location = 1) in vec3 vertexNormal;\r\n\
		layout(location = 2) in vec2 texCoord;\r\n\
		layout(location = 3) in vec4 vertexColor;\r\n\
		out vec3 normal;\r\n\
		out vec4 vPosition;\r\n\
		uniform mat4 projectionView;\r\n\
		uniform mat4 modelTransform;\r\n\
		void main() {\r\n\
			vPosition = modelTransform * vec4(vertexPosition, 1.0);\r\n\
			gl_Position = projectionView * vPosition;\r\n\
			normal = normalize(vec3(modelTransform * vec4(vertexNormal, 0)));\r\n\
		}";
	std::string _particleShaderFragmentSource = "#version 300 es\r\n\
		precision highp float;\r\n\
		in vec3 normal;\r\n\
		in vec4 vPosition;\r\n\
		out vec4 outputColor;\r\n\
		uniform vec3 cameraPosition;\r\n\
		uniform vec4 particleColor;\r\n\
		uniform float angle;\r\n\
		uniform float isFlickering;\r\n\
		uniform vec3 vDiffuse;\r\n\
		uniform vec3 vSpecular;\r\n\
		uniform float vSpecularExponent;\r\n\
		uniform vec3 lightDirection;\r\n\
		float randomExponent(vec2 config) {\r\n\
		    return fract(sin(dot(config.xy, vec2(12.9898, 78.233))) * 43758.5453);\r\n\
		}\r\n\
		void main(){\r\n\
			vec3 viewDir = normalize(cameraPosition - vPosition.xyz);\r\n\
		    vec3 sunDir = normalize(lightDirection);\r\n\
		    vec3 reflectionVector =  normalize(2.0 * dot(normal, -sunDir) * normal + sunDir);\r\n\
		    float brightness = clamp(1.1 * dot(normal, -sunDir), 0.0, 1.0);\r\n\
		    float exponent = randomExponent(vec2(1.0, pow(angle, 1.5)));\r\n\
		    vec3 randomColor = clamp(vec3(0.5, 0.5, 0.0) + vec3(sin(angle/10.0), cos(angle/20.0 + vPosition.y), sin(vPosition.z)), 0.0, 1.0);\r\n\
		    vec3 modifiedDiffuseColor = (1.0 - isFlickering) * particleColor.xyz + isFlickering * randomColor;\r\n\
		    vec3 specularComponent = clamp(modifiedDiffuseColor * vSpecular * pow(dot(reflectionVector, viewDir), vSpecularExponent), 0.0, 1.0);\r\n\
		    vec3 color = clamp((modifiedDiffuseColor + specularComponent) * brightness, 0.0, 1.0);\r\n\
		    outputColor = vec4( color , 1.0);\r\n\
		}";
	std::vector<GameUtilities::Particle> _particleData;

	//**************************************** Private Method Declaractions **********************************
	~AppGLCanvas(void);
	void _applyTextShaderSettings(void);
	void _applyParticleShaderSettings(void);
	/*
	* @brief Handles the main event loop in idle time.
	*/
	bool _finishIdleTask(unsigned long deltaTime);
	/*
	* @brief Initializes default member variables of the OpenGL canvas instance and compiles generic shader elements.
	*/
	void _initDefaultVariables(void);
	bool _offlineGameLoop(GameUtilities::GameState* gameState, unsigned long deltaTime);
	bool _processPendingResourceData(void);
	/*
	* @brief Renders the default background at the start of the application.
	*/
	void _renderDefaultScene(void);
	/*
	* @brief Simulates the random motion of brownian particles at each frame call.
	*/
	void _updateBrownianParticleMotion(void);
	void _updateCameraView(void);
};
#endif // !APP_GL_CANVAS_H
