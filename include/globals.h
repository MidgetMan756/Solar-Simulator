// globals.h
// Shared state for the SolarSim application.
#pragma once

#include <random>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace SolarSim {

class Mass;

// Simulation configuration
extern double timeStepMult;
extern double zoomFactor;
extern double camX;
extern double camY;
extern double screenScale;

// Input state
extern bool isLeftMouseButtonDown;
extern bool isRightMouseButtonDown;
extern bool isMiddleMouseButtonDown;
extern int massType;
extern double startxpos;
extern double startypos;
extern double lastMouseX;
extern double lastMouseY;
extern bool isCameraFollowMass;
extern bool clickedExistingMass;
extern int selectedMassIndex;

// GLFW / OpenGL objects
extern GLFWwindow* window;
extern unsigned int shaderProgram;
extern unsigned int textShaderProgram;
extern unsigned int textVAO;
extern unsigned int textVBO;
extern GLint textScreenUniform;
extern GLint textColorUniform;

// Text rendering state
extern int textVertexCount;
extern std::vector<float> textVertices;
extern bool showTextOverlay;
extern std::string overlayText;
extern float overlayScale;
extern float overlayMargin;
extern float timeOverlayScale;
extern float timeOverlayMargin;
extern std::string timeOverlayText;
extern std::vector<unsigned char> textScratchBuffer;
inline constexpr size_t kEasyFontBytesPerChar = 288;

// Simulation collections
extern std::vector<Mass> massesVector;
extern std::vector<std::string> celestialBodies;

// Utilities
extern std::mt19937 randomGenerator;

} // namespace SolarSim
