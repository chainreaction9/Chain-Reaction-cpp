#pragma once
#ifndef TEXT_H
#define TEXT_H

#include "PrecompiledHeader.h"
#include "Shaders.h"
#include "MeshGenerator.h"

class Text{
	public:
		/**
		* @brief Default constructor for Text object.
		* @param {float} FontSize: size of the font.
		* @param {float} width: space between each letter.
		* @param {float} height: space between each line.
		* @param {const unordered_map<string, MeshData*>&} charset: an unordered map between 26 alphabet names and pointers to their MeshData.
		*/
		Text(float FontSize, float width, float height, const std::unordered_map<std::string, MeshData*>& charset);
		/**
		* @brief A constant method which draws a sentence without line break.
		* @param {Shaders&} shaderProgram: reference to the Shaders object which handles the text rendering. It is eventually passed on to the MeshData object associated with each of the alphabets for handling material properties.
		* @param {GLint} modelMatrixUniformLocation: <GLint> location of modelMatrix uniform variable in the Shader program specified by shaderProgram parameter.
		* @param {string} text: a text for rendering which does not contain new line characters.
		* @param {glm::vec3} position: position of the text object in 3D OpenGL world.
		* @param {glm::vec3} rotation: angle of rotation with respect to x-axis, y-axis and z-axis, respectively.
		* @param {string} horizontalAlign: possible values: 'right', 'center', 'left'. Text alignment depends on the given value.
		*/
		void DrawText(Shaders& shaderProgram, GLint modelMatrixUniformLocation = -1, std::string text = "", glm::vec3 position = glm::vec3(0,0,0), glm::vec3 rotation = glm::vec3(0, 0, 0), std::string horizontalAlign = "center") const;
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
		void DrawLine(Shaders& shaderProgram, GLint modelMatrixUniformLocation = -1, std::string textWithMultipleLines = "", glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 rotation = glm::vec3(0, 0, 0), std::string horizontalAlign = "center", std::string verticalAlign = "center") const;
		inline float GetFontSize(void) { return this->_fontSize; }
		inline float GetWidth(void) { return this->_width; }
		inline float GetHeight(void) { return this->_height; }
		inline void SetFontSize(float size) { this->_fontSize = size; }
		inline void SetWidth(float width) { this->_width = width; }
		inline void SetHeight(float height) { this->_height = height; }
		~Text(void);

	private:
		float _fontSize = 0, _width = 0, _height = 0;
		std::vector<MeshData*> _fontData; //stores the mesh data (ordered as in a-z) corresponding to each of the 26 alphabets. 
};
#endif