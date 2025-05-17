#include "keyProcessing.h"
#include "GLFW/glfw3.h"

#include <iostream>

KeyProcessing::KeyProcessing(GLFWwindow* window) : window(window) {}

KeyProcessing::~KeyProcessing() {}

void KeyProcessing::processInput() {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		std::cout << "window killed" << std::endl;
		glfwSetWindowShouldClose(window, true);
	}
}
