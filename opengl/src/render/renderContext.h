#ifndef RENDER_CONTEXT_H
#define RENDER_CONTEXT_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <sstream>

class RenderContext {
public:
	RenderContext(GLFWwindow* window);
	~RenderContext();

	void render();

private:
	void loadTexture();

	// returns ids of these objects
	unsigned int vertexShader();
	unsigned int fragmentShader();
	unsigned int shaderProgram();

	std::string parseShaderFile(const std::string& file_path);

	GLFWwindow* window;
	unsigned int shader_program;
	unsigned int vertex_buffer_object;
	unsigned int vertex_array_object;
	unsigned int element_buffer_object;
	unsigned int texture;
};

#endif
