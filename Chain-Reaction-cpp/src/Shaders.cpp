#include "PrecompiledHeader.h" //precompiled header
#include "Shaders.h"

Shaders::Shaders() : _programID(0), _VertexShader(""), _FragmentShader(""){
	this->_currentMaterial.name = "";
}
Shaders::~Shaders() {
	if (_programID != 0) {
		wxLogDebug(wxString::Format("[Chain-Reaction] Freed memory allocated to shader program %d at address %p ...", _programID, &_programID));
		glDeleteProgram(_programID);
	}
}
bool Shaders::compileShaders(const char* VertSource, const char* FragSource, bool rawSource) {
	if (rawSource) {  //VertSource and FragSource are raw source code
		_VertexShader = std::string(VertSource);
		_FragmentShader = std::string(FragSource);
	}
	else { //VertSource and FragSource are file paths
		wxFile file;
		wxString VertFileData, FragFileData;
		file.Open(wxString(VertSource));
		if (file.IsOpened()) {
			if (!file.ReadAll(&VertFileData)) {
				std::string errorCode = (std::string) wxString::Format("Error code: %d.", file.GetLastError()).c_str();
				wxMessageBox("Failed to read file '"+std::string(VertSource)+"'. "+errorCode, wxT("Shader Creation Error"), wxOK | wxICON_ERROR);
				return false;
			}
			_VertexShader.assign(VertFileData.c_str());
			file.Close();
		}
		else {
			std::string errorCode = (std::string) wxString::Format("Error code: %d.", file.GetLastError()).c_str();
			wxMessageBox("Failed to open file '" + std::string(VertSource) + "'. " + errorCode, wxT("Shader Creation Error"), wxOK | wxICON_ERROR);
			return false;
		}
		file.Open(wxString(FragSource));
		if (file.IsOpened()) {
			if (!file.ReadAll(&FragFileData)) {
				std::string errorCode = (std::string) wxString::Format("Error code: %d.", file.GetLastError()).c_str();
				wxMessageBox("Failed to read file '" + std::string(FragSource) + "'. " + errorCode, wxT("Shader Creation Error"), wxOK | wxICON_ERROR);
				return false;
			}
			_FragmentShader.assign(FragFileData.c_str());
			file.Close();
		}
		else {
			std::string errorCode = (std::string) wxString::Format("Error code: %d.", file.GetLastError()).c_str();
			wxMessageBox("Failed to open file '" + std::string(FragSource) + "'. " + errorCode, wxT("Shader Creation Error"), wxOK | wxICON_ERROR);
			return false;
		}
	}
	//Creating and compiling vertex shader

	GLuint _vertID = glCreateShader(GL_VERTEX_SHADER);
	if (_vertID == 0) {
		wxMessageBox("Failed to create vertex shader ID!", wxT("Shader Creation Error"), wxOK | wxICON_ERROR);
		return false;
	}
	const char* contentsPtr = _VertexShader.c_str();
	glShaderSource(_vertID, 1, &contentsPtr, nullptr);
	glCompileShader(_vertID);
	GLint isCompiled = 0;
	glGetShaderiv(_vertID, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(_vertID, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<char> errorLog(maxLength);
		glGetShaderInfoLog(_vertID, maxLength, &maxLength, &errorLog[0]);
		std::string errorString((const char*)&(errorLog[0]), (size_t)errorLog.size());
		wxMessageBox("Failed to compile vertex shader: \n\n" + errorString, wxT("Shader Compilation Error"), wxOK | wxICON_ERROR);
		glDeleteShader(_vertID);
		return false;
	}
	//Creating and compiling fragment shader
	GLuint _fragID = glCreateShader(GL_FRAGMENT_SHADER);
	if (_fragID == 0) {
		wxMessageBox("Failed to create fragment shader ID!", wxT("Shader Creation Error"), wxOK | wxICON_ERROR);
		glDeleteShader(_vertID);
		return false;
	}

	contentsPtr = _FragmentShader.c_str();
	glShaderSource(_fragID, 1, &contentsPtr, nullptr);
	glCompileShader(_fragID);
	isCompiled = 0;
	glGetShaderiv(_fragID, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(_fragID, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<char> errorLog(maxLength);
		glGetShaderInfoLog(_fragID, maxLength, &maxLength, &errorLog[0]);
		std::string errorString((const char*)&(errorLog[0]), (size_t)errorLog.size());
		wxMessageBox("Failed to compile fragment shader: \n\n" + errorString, wxT("Shader Compilation Error"), wxOK | wxICON_ERROR);
		glDeleteShader(_vertID);
		glDeleteShader(_fragID);
		return false;
	}

	//Creating and linking final program
	if (_programID != 0) {		//delete old program
		glDeleteProgram(_programID);
		_programID = 0;
	}
	_programID = glCreateProgram();
	if (_programID == 0) {
		wxMessageBox("Failed to create program ID!", wxT("Shader Creation Error"), wxOK | wxICON_ERROR);
		glDeleteShader(_vertID);
		glDeleteShader(_fragID);
		return false;
	}

	glAttachShader(_programID, _vertID);
	glAttachShader(_programID, _fragID);
	glLinkProgram(_programID);
	GLint isLinked = 0;
	glGetProgramiv(_programID, GL_LINK_STATUS, (int*)&isLinked);
	if (isLinked == GL_FALSE) {
		GLint maxLength = 0;
		glGetProgramiv(_programID, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(_programID, maxLength, &maxLength, &infoLog[0]);
		std::string errorString((const char*)&infoLog[0], (size_t)infoLog.size());
		wxMessageBox("Failed to link program: \n\n" + errorString, wxT("Program Linking Error"), wxOK | wxICON_ERROR);
		glDeleteProgram(_programID);
		glDeleteShader(_vertID);
		glDeleteShader(_fragID);
		return false;
	}
	glDetachShader(_programID, _vertID);
	glDetachShader(_programID, _fragID);
	glDeleteShader(_vertID);
	glDeleteShader(_fragID);

	this->_detectUniforms();
	wxLogDebug(wxString::Format("[Chain-Reaction] Created shader program %d at address %p ...", _programID, &_programID));
	return true;
}
bool Shaders::applyMaterial(objl::Material material, const char* diffuseColorUniformName, const char* specularColorUniformName, const char* specularExponentUniformName){
	if (!(this->_currentMaterial.name.compare(material.name))) return true;
	bool success = false;
	this->use();
	GLint location = this->getUniformLocation(diffuseColorUniformName);
	if (location != -1) {
		glUniform3f(location, material.Kd.X, material.Kd.Y, material.Kd.Z);
		success = true;
	}
	location = this->getUniformLocation(specularColorUniformName);
	if (location != -1) {
		glUniform3f(location, material.Ks.X, material.Ks.Y, material.Ks.Z);
		success = true;
	}
	location = this->getUniformLocation(specularExponentUniformName);
	if (location != -1) {
		glUniform1f(location, material.Ns);
		success = true;
	}
	this->_currentMaterial = material;
	return success;
}
GLint Shaders::getUniformLocation(const char* uniformName) const {
	auto pos = _uniformList.find(uniformName);
	if (pos != _uniformList.end()) {
		return pos->second;
	}
	else {
		return -1;
	}
}
void Shaders::_detectUniforms() {
	GLint totalUniforms = 0;
	GLint maxLength = 0;
	glGetProgramiv(this->_programID, GL_ACTIVE_UNIFORMS, &totalUniforms);
	glGetProgramiv(this->_programID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);
	std::string uniformName;
	for (int i = 0; i < totalUniforms; i++) {
		glGetActiveUniformName(this->_programID, i, maxLength, NULL, &uniformName[0]);
		GLint location = glGetUniformLocation(_programID, uniformName.c_str());
		if (location < 0) continue;
		_uniformList[uniformName.c_str()] = location;
	}
}
void Shaders::use() const {
	glUseProgram(_programID);
}
void Shaders::unuse() const {
	glUseProgram(0);
}