#include "PrecompiledHeader.h" //precompiled header
#include "Text.h"
/**
* @brief Default constructor for Text object.
* @param {float} FontSize: size of the font.
* @param {float} width: space between each letter.
* @param {float} height: space between each line.
* @param {const unordered_map<string, MeshData*>&} charset: an unordered map between 26 alphabet names (in capital letter) and pointers to their MeshData.
*/
Text::Text(float FontSize, float width, float height, const std::unordered_map<std::string, MeshData*>& charset){
	this->_fontSize = FontSize;
	this->_width = width;
	this->_height = height;
	this->_fontData.resize(26); //Reserve memories for mesh data corresponding to each of the 26 letters.
	int collectedAlphabet = 0;
	for (const auto& alphabet : charset) {
		std::string alphabetName = alphabet.first;
		if (alphabetName.size() == 1) { //Collect the meshdata for the names containing single alphabet.
			int letterIndex = (int)alphabetName[0] - 65;
			MeshData* entryData = alphabet.second;
			this->_fontData[letterIndex] = entryData;
			collectedAlphabet++;
		}
		if (collectedAlphabet == 26) break; //All of the 26 alphabet mesh data was collected. No need to traverse the charset anymore.
	}	
}
/**
* @brief A constant method which draws a sentence without line break.
* @param {Shaders&} shaderProgram: reference to the Shaders object which handles the text rendering. It is eventually passed on to the MeshData object associated with each of the alphabets for handling material properties.
* @param {GLint} modelMatrixUniformLocation: <GLint> location of modelMatrix uniform variable in the Shader program specified by shaderProgram parameter.
* @param {string} text: a text for rendering which does not contain new line characters.
* @param {glm::vec3} position: position of the text object in 3D OpenGL world.
* @param {glm::vec3} rotation: angle of rotation with respect to x-axis, y-axis and z-axis, respectively.
* @param {string} horizontalAlign: possible values: 'right', 'center', 'left'. Text alignment depends on the given value.
*/
void Text::DrawText(Shaders& shaderProgram, GLint modelMatrixUniformLocation, std::string text, glm::vec3 position, glm::vec3 rotation, std::string horizontalAlign) const{
	glm::mat4 _model = glm::translate(glm::mat4(1.0f), glm::vec3(position.x,position.y,position.z));
	_model = glm::rotate(_model, (float)glm::radians(-rotation.x), glm::vec3(1, 0, 0));
	_model = glm::rotate(_model, (float)glm::radians(-rotation.y), glm::vec3(0, 1, 0));
	_model = glm::rotate(_model, (float)glm::radians(-rotation.z), glm::vec3(0, 0, 1));
	glm::mat4 _model1 = glm::scale(_model, glm::vec3(this->_fontSize, this->_fontSize, this->_fontSize));
	if (horizontalAlign.compare("right") != 0){
		if (horizontalAlign.compare("center") == 0)
			_model1 = glm::translate(_model1, glm::vec3(-0.5 * this->_width * ((int)text.size() - 1), 0, 0));
		for(int i = 0; i < (int) text.size(); i++){
			if (text[i] == ' '){
				_model1 = glm::translate(_model1, glm::vec3(0.7 * this->_width, 0, 0));
				continue;
			}
			glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, &_model1[0][0]);
			this->_fontData[((int)text[i]) - 65]->render(shaderProgram);
			_model1 = glm::translate(_model1, glm::vec3(this->_width, 0, 0));
		}
	}
	else{
		for(int i = (int) text.size() - 1; i >= 0; i--){
			if (text[i] == ' '){
				_model1 = glm::translate(_model1, glm::vec3(-0.7 * this->_width, 0, 0));
				continue;
			}
			glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, &_model1[0][0]);
			this->_fontData[((int) text[i]) - 65]->render(shaderProgram);
			_model1 = glm::translate(_model1, glm::vec3(-this->_width, 0, 0));
		}
	}
}
/**
* @brief A constant method which draws a sentence containing multiple line breaks.
* @param {Shaders&} shaderProgram: reference to the Shaders object which handles the text rendering. It is eventually passed on to the MeshData object associated with each of the alphabets for handling material properties.
* @param {GLint} modelMatrixUniformLocation: <GLint> location of modelMatrix uniform variable in the Shader program specified by shaderProgram parameter.
* @param {string} text: a text for rendering which must contain at least one new line character.
* @param {glm::vec3} position: position of the text object in 3D OpenGL world.
* @param {glm::vec3} rotation: angle of rotation with respect to x-axis, y-axis and z-axis, respectively.
* @param {string} horizontalAlign: possible values: 'right', 'center', 'left'. Has a default value of 'center'. Text alignment depends on the given value.
* @param {string} verticalAlign: possible values: 'top', 'center', 'bottom'. Has a default value of 'center'. Text alignment depends on the given value.
*/
void Text::DrawLine(Shaders& shaderProgram, GLint modelMatrixUniformLocation, std::string textWithMultipleLines, glm::vec3 position, glm::vec3 rotation, std::string horizontalAlign, std::string verticalAlign) const{
	std::vector<std::string> line_list;
	float x = position.x;
	float y = position.y;
	float z = position.z;
	std::string line;
	if (textWithMultipleLines.find('\n') != std::string::npos){
		std::istringstream temp(textWithMultipleLines);
		while (std::getline(temp,line)) line_list.push_back(line);
	}
		
	if (verticalAlign.compare("center") == 0) {		
		y += 0.3f*this->_height*((int) line_list.size() - 1);
		for (int i = 0; i < (int) line_list.size(); i++){
			line = line_list[i];
			this->DrawText(shaderProgram, modelMatrixUniformLocation, line, glm::vec3(x, y, z), rotation, horizontalAlign);
			y -= 0.6f * this->_height;
		}
	}
	else if (verticalAlign.compare("top") == 0){
			
		for (int i = 0; i < (int) line_list.size(); i++){
			line = line_list[i];
			this->DrawText(shaderProgram, modelMatrixUniformLocation, line, glm::vec3(x, y, z), rotation, horizontalAlign);
			y -= 0.6f * this->_height;
		}
	}
	else{
		for (int i = ((int) line_list.size() - 1); i >= 0 ; i--){
			line = line_list[i];
			this->DrawText(shaderProgram, modelMatrixUniformLocation, line, glm::vec3(x, y, z), rotation, horizontalAlign);
			y += 0.6f * this->_height;
		}
	}
}

Text::~Text(void){
}
