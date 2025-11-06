// mass.h
// Definition of the Mass class encapsulating renderable celestial bodies.
#pragma once

#include <string>

#include <glad/glad.h>

namespace SolarSim {

class Mass {
public:
    // BTW this is almost never used, but it could be fun if I could
    // figure out how THE FUCK to get text to work within GLFW
    std::string name;
    float mass = 0.0f;
    float radius = 0.0f;

    double x = 0.0f;
    double y = 0.0f;
    double vx = 0.0f;
    double vy = 0.0f;
    double ax = 0.0f;
    double ay = 0.0f;

    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;

    unsigned int VAO = 0;
    unsigned int VBO = 0;

    // Number of segments approximating the planet disc (360 sided polygon).
    int numOfVertices = 360;

    void init();
    void updateVertices();
    void draw(unsigned int shaderProgram);
    void calcAcceleration(double otherMass, double otherX, double otherY);
    void calcVelocity();
    void calcNewPos();
};

bool checkCollision(const Mass& m1, const Mass& m2);
void resolveCollision(Mass& m1, Mass& m2);
void mergeMasses(Mass& m1, Mass& m2);

} // namespace SolarSim
