#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "keyProcessing.h"
#include "render/renderContext.h"


int main(void) {

	// initialization for glfw instance
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // sets 3.3 (MAJOR.MINOR) version for opengl
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // macos compatability

	GLFWwindow* window = glfwCreateWindow(800, 600, "Triangle", NULL, NULL);
	if (!window) {
		std::cout << "Failed to create glfw window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); // where everything will be rendered to 
	
	// initialize glad as a manager for func pointers of opengl
	if (!gladLoadGL(glfwGetProcAddress)) { 
		std::cout << "Failed to initialize GlAD" << std::endl;
		return -1;
	}

	// setting viewport to macos set value
	int fbWidth, fbHeight;
	glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
	glViewport(0,0,fbWidth,fbHeight);
	// allows for user resize of window
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		glViewport(0,0,width,height);
	});

	KeyProcessing* kp = new KeyProcessing(window);
	RenderContext* renderContext = new RenderContext(window); 

	// render loop
	while (!glfwWindowShouldClose(window)) {
		// input handling before every fresh frame
		kp->processInput();

		// rendering
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // clears window for next frame
		glClear(GL_COLOR_BUFFER_BIT);

		// renders all content
		renderContext->render();

		glfwSwapBuffers(window); // swaps pixel buffers for cleaner frames
		glfwPollEvents(); // registers mouse or keyboard events
	}

	// cleanup
	delete kp;
	delete renderContext;
	glfwTerminate(); // rids computer of all rosource allocated by glfw
	return 0;
}
