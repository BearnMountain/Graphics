#include "renderContext.h"
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_implementation.h"

RenderContext::RenderContext(GLFWwindow* window) : window(window) {
	// coords, colors, texture coords
	
	float vertices[] = {
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f, //  1.0f, 1.0f, top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f, //  1.0f, 0.0f, bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f, //  0.0f, 0.0f, bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f, //  0.0f, 1.0f  top left 
	};
	unsigned int indices[] = {
		0,1,3, // first triangle
		1,2,3  // second triangle
	};

	shader_program = shaderProgram();

	// generate ids for vertex objects
	glGenVertexArrays(1, &vertex_array_object);
	glGenBuffers(1, &vertex_buffer_object);
	glGenBuffers(1, &element_buffer_object);

	// 0. bind vertex array object
	glBindVertexArray(vertex_array_object);

	// 1. copies vertices array into a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object); // binds buffer so changes can be made 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// 2. sets the vertex attributes pointers
	// cordinate attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // location 0 of vert shader, vec3 size of vertices, float values for each, no normalization(-1 to 1), size of each vertice, offset from start of vertices
	glEnableVertexAttribArray(0);

	// color attributes
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
/*
	// texture attributes
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);  
*/
	// loadTexture();

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

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe
	// glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(vertex_array_object);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0); // unbind VAO after drawing
	
}

void RenderContext::loadTexture() {
	// binding texture to gpu
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load("src/render/textures/wall_image.jpg", &width, &height, &nrChannels, 0);

	// generate texture using loaded image
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cout << "Failed to load texture" << std::endl;
	}

	// cleanup
	stbi_image_free(data);
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
