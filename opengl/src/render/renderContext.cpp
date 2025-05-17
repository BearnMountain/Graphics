#include "renderContext.h"
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <fstream>
#include <iostream>
#include <stdexcept>

RenderContext::RenderContext(GLFWwindow* window) : window(window) {
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f,  0.5f, 0.0f
	};

	shader_program = shaderProgram();

	// generate ids for vertex objects
	glGenVertexArrays(1, &vertex_array_object);
	glGenBuffers(1, &vertex_buffer_object);

	// 0. bind vertex array object
	glBindVertexArray(vertex_array_object);
	// 1. copies vertices array into a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object); // binds buffer so changes can be made
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 	
	// 2. sets the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // location 0 of vert shader, vec3 size of vertices, float values for each, no normalization(-1 to 1), size of each vertice, offset from start of vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

RenderContext::~RenderContext() {
	glDeleteVertexArrays(1, &vertex_array_object);
	glDeleteBuffers(1, &vertex_buffer_object);
	glDeleteProgram(shader_program);
}

void RenderContext::render() {
	 // creates the program that interprets vertice data, loaded to gpu and just needs opengl calls to be triggered
	glUseProgram(shader_program);
	glBindVertexArray(vertex_array_object);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0); // unbind VAO after drawing
}

unsigned int RenderContext::shaderProgram() {
	// creating shaders
	unsigned int vertex_shader = vertexShader();
	unsigned int fragment_shader = fragmentShader();

	unsigned int shader_program = glCreateProgram();
	if (!shader_program) {
		std::cerr << "Failed to create shader program" << std::endl;
		return 0;
	}
	
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	int success;
    char infoLog[512];
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        // Handle error, maybe throw exception
        glDeleteProgram(shader_program);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return 0;
    }

	// now i can safely delete the shaders as the compiled info is copied in
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return shader_program;
}

unsigned int RenderContext::vertexShader() {
	// string containing the shader execution code
	std::string parsed_shader = parseShaderFile("src/render/vertexShader.vert");
	const char* vertex_shader_source = parsed_shader.c_str();
	
	unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader); // compiles file to create full shader

	// checks errors 
	int success;
    char infoLog[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        std::cerr << "Vertex Shader Compilation Error: " << infoLog << std::endl;
        glDeleteShader(vertex_shader);
        return 0; // Or throw an exception
    }

	return vertex_shader;
}

unsigned int RenderContext::fragmentShader() {
	std::string parsed_shader = parseShaderFile("src/render/fragmentShader.frag");
	const char* fragment_shader_source = parsed_shader.c_str();

	unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader); // takes glsl and compiles it
	
	// checks errors
	int success;
    char infoLog[512];
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
        std::cerr << "Fragment Shader Compilation Error: " << infoLog << std::endl;
        glDeleteShader(fragment_shader);
        return 0; // Or throw an exception
    }
	
	return fragment_shader;
}



std::string RenderContext::parseShaderFile(const std::string& file_path) {
	std::ifstream file(file_path);
	if (!file) {
		throw std::runtime_error("incorrect filepath:" + file_path);
	}
	std::stringstream buffer;
	buffer << file.rdbuf();

	file.close();
	return buffer.str();
}
