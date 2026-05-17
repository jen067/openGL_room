#pragma once
#include <glfw/include/GLFW/glfw3.h>

// 滑鼠按下
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

// 滑鼠移動
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);

// 視窗大小改變
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

// 鍵盤按下
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);