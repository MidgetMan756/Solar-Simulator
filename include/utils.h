// utils.h
// Simulation helper routines and math utilities.
#pragma once

#include <string>
#include <vector>

struct GLFWwindow;

namespace SolarSim {

double average(const std::vector<double>& v);
float randomFloat(float min, float max);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void screenToNDC(double sx, double sy, int width, int height, double& nx, double& ny);
void ndcToWorld(double nx, double ny, double& wx, double& wy);
void screenToWorld(double sx, double sy, int width, int height, double& wx, double& wy);

std::string getRandomBodyName();
std::string formatScientific(double value, int precision = 3);

} // namespace SolarSim
