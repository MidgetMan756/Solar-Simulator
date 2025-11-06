#include "globals.h"

#include "constants.h"
#include "mass.h"

namespace SolarSim {

// Simulation parameters
double timeStepMult = 600.0;
double zoomFactor = 0.9;
double camX = 0.0;
double camY = 0.0;
double screenScale = 1.0 / (Constants::earthMoonDistance * 2) * zoomFactor;

// Input state
bool isLeftMouseButtonDown = false;
bool isRightMouseButtonDown = false;
bool isMiddleMouseButtonDown = false;
int massType = 0;
double startxpos = 0.0;
double startypos = 0.0;
double lastMouseX = 0.0;
double lastMouseY = 0.0;
bool isCameraFollowMass = false;
bool clickedExistingMass = false;
int selectedMassIndex = -1;

// OpenGL handles
GLFWwindow* window = nullptr;
unsigned int shaderProgram = 0;
unsigned int textShaderProgram = 0;
unsigned int textVAO = 0;
unsigned int textVBO = 0;
GLint textScreenUniform = -1;
GLint textColorUniform = -1;

// Text rendering buffers
int textVertexCount = 0;
std::vector<float> textVertices;
bool showTextOverlay = false;
std::string overlayText;
float overlayScale = 3.0f;
float overlayMargin = 24.0f;
float timeOverlayScale = 2.6f;
float timeOverlayMargin = 24.0f;
std::string timeOverlayText;
std::vector<unsigned char> textScratchBuffer;

// Simulation collections
std::vector<Mass> massesVector;
std::vector<std::string> celestialBodies = {
    // Real exoplanets
    "Kepler-22b", "Kepler-62f", "Kepler-69c", "Kepler-186f", "Kepler-442b", "Kepler-452b",
    "Kepler-1649c", "Kepler-438b", "Kepler-62e", "Kepler-62c", "Kepler-283c",
    "TOI-700d", "TOI-1452b", "TOI-1338b", "TOI-700e", "TOI-715b",
    "LHS 1140b", "LHS 475b", "LHS 3844b",
    "TRAPPIST-1b", "TRAPPIST-1c", "TRAPPIST-1d", "TRAPPIST-1e", "TRAPPIST-1f", "TRAPPIST-1g", "TRAPPIST-1h",
    "Proxima b", "Proxima c", "Ross 128b", "Gliese 667Cc", "Gliese 581g",
    "Gliese 876d", "Gliese 1214b", "Gliese 163c", "HD 209458b", "HD 189733b", "HD 40307g",
    "HD 85512b", "HD 28185b", "HD 69830d", "HD 21749c", "HD 40307b", "HD 40307c",
    "51 Pegasi b", "55 Cancri e", "61 Virginis b", "Upsilon Andromedae b",
    "Tau Ceti e", "Tau Ceti f", "Epsilon Eridani b", "Epsilon Indi Ab",
    "K2-18b", "K2-155d", "K2-72e", "K2-72f", "K2-288Bb", "K2-315b",

    // Fictional but scientifically styled
    "HD 42011b", "K2-901c", "Kepler-942b", "Kepler-1101d",
    "Gliese 942f", "TRAPPIST-3e", "Proxima d", "LHS 442c",
    "Tau Ceti g", "Epsilon Lyrae b", "HD 40307e",
    "Ross 294d", "OGLE-TR-56b", "XO-1b", "WASP-12b", "WASP-39b", "WASP-76b",
    "GJ 1132b", "GJ 357d", "GJ 504b", "GJ 9827d", "GJ 229Ac",
    "OGLE-2005-BLG-390Lb", "OGLE-2017-BLG-1522b",
    "PSR B1257+12b", "PSR B1257+12c", "PSR B1620-26b",
    "CoRoT-7b", "CoRoT-9b", "CoRoT-24c",
    "HAT-P-11b", "HAT-P-26b", "HAT-P-32b", "HAT-P-7b",
    "HIP 65426b", "HIP 41378f", "HIP 70849b",

    // Plausible procedural/catalogue-style (fictional)
    "HD 5012d", "HD 7231c", "Gliese 775b", "Kepler-2339f", "Kepler-3902e",
    "TOI-995b", "TOI-1423d", "TRAPPIST-9c", "K2-404e", "K2-601b",
    "OGLE-290b", "OGLE-1024c", "WASP-98c", "WASP-172b", "GJ 4123b",
    "HIP 99231c", "HIP 12008b", "HD 41023d"
};

// Random number generator seeded once (used for mass creation)
std::mt19937 randomGenerator{std::random_device{}()};

} // namespace SolarSim
