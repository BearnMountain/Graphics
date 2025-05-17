#ifndef KEY_PROCESSING_H
#define KEY_PROCESSING_H

#include "GLFW/glfw3.h"
class KeyProcessing {
public:
	KeyProcessing(GLFWwindow* window);
	~KeyProcessing();

	void processInput();

private:
	GLFWwindow* window;
};

#endif
