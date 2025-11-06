// input.h
// Handles mouse and keyboard processing for the simulation.
#pragma once

struct GLFWwindow;

namespace SolarSim {

int processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

} // namespace SolarSim
