#include "utils.h"

#include <iomanip>
#include <numeric>
#include <random>
#include <sstream>

#include <glad/glad.h>

#include "globals.h"

namespace SolarSim {

double average(const std::vector<double>& v) {
    if (v.empty()) return 0.0; // if the vector is empty
    double sum = std::accumulate(v.begin(), v.end(), 0.0); // get sum
    return sum / static_cast<double>(v.size()); // return sum / size
}

float randomFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(randomGenerator);
}

void framebuffer_size_callback(GLFWwindow* /*window*/, int width, int height) {
    // Changes the window's viewport size
    glViewport(0, 0, width, height);
}

void screenToNDC(double sx, double sy, int width, int height, double& nx, double& ny) {
    nx = (sx / width) * 2.0 - 1.0;
    ny = 1.0 - (sy / height) * 2.0; // flip Y (GLFW top-left -> NDC top)
}

void ndcToWorld(double nx, double ny, double& wx, double& wy) {
    // inverse of world->NDC: draw = world * screenScale
    wx = nx / screenScale + camX;
    wy = ny / screenScale + camY;
}

void screenToWorld(double sx, double sy, int width, int height, double& wx, double& wy) {
    double nx, ny;
    screenToNDC(sx, sy, width, height, nx, ny);
    ndcToWorld(nx, ny, wx, wy);
}

std::string getRandomBodyName() {
    if (celestialBodies.empty()) return "[UNKNOWN]";
    std::uniform_int_distribution<size_t> dist(0, celestialBodies.size() - 1);
    return celestialBodies[dist(randomGenerator)];
}

std::string formatScientific(double value, int precision) {
    std::ostringstream oss;
    oss << std::scientific << std::setprecision(precision) << value;
    return oss.str();
}

} // namespace SolarSim
