#ifndef SHADERS_H
#define SHADERS_H

#include "PrecompiledHeader.h"
#include "ObjLoader.h"

class Shaders{
public:
	/*
	* @brief Default constructor of Shaders object. It only supports vertex and fragment shaders.
	*/
	Shaders();
	/*
	* @brief Default destructor method of Shaders object.
	* @brief Frees the memory associated with the program ID.
	*/
	~Shaders();
	/*
	* @brief Creates vertex and fragment shaders from raw source or text files, accordingly compiles and links them.
	* @param {const char*} VertSource: raw source code for the vertex shader in case the parameter rawSource is set to true, otherwise path to the vertex shader file.
	* @param {const char*} FragSource: raw source code for the fragment shader in case the parameter rawSource is set to true, otherwise path to the fragment shader file.
	* @param {bool} rawSource: boolean flag to tell the compiler if source code for the shaders are passed in raw form or as file paths.
	* @returns true if shader processing was successful, false otherwise.
	*/
	bool compileShaders(const char* VertSource, const char* FragSource, bool rawSource = false);
	/*
	* @brief Updates the uniform variables associated with material
	* properties (e.g., diffuse color, ambient color, specular color, specular exponent etc.) in the fragment shader.
	* @param {obj::Material} material: a Material object containing associated material properties.
	* @param {const char*} diffuseColorUniformName: name of the uniform variable (vec3) for diffuse color in the shader.
	* @param {const char*} specularColorUniformName: name of the uniform variable (vec3) for specular color in the shader.
	* @param {const char*} specularExponentUniformName: name of the uniform variable (float) for specular exponent in the shader.
	* @returns true if at least one material property is updated, false otherwise.
	*/
	bool applyMaterial(objl::Material material, const char* diffuseColorUniformName = "vDiffuse", const char* specularColorUniformName = "vSpecular", const char* specularExponentUniformName = "vSpecularExponent");
	//Binds the shader program.
	void use() const;
	//Unbinds the shader program.
	void unuse() const;
	/*
	* @brief Searches for the <GLint> location of a uniform variable in the shader program.
	* @param {const char*} uniformName: name of the uniform variable.
	* @returns {GLint} location of the uniform variable if found, -1 otherwise.
	*/
	GLint getUniformLocation(const char* uniformName) const;
	// Returns the program ID.
	inline GLuint getID() const { return _programID; }

private:
	std::unordered_map<std::string, GLuint> _uniformList; // A private map containing name of the uniform variables and their locations.
	std::string _VertexShader, _FragmentShader;
	GLuint _programID;
	objl::Material _currentMaterial;
	/*
	* @brief Collects all uniform variable names from the program and store them in a private map <name, location> for handy usage.
	*/
	void _detectUniforms();
};
#endif //!SHADERS_H