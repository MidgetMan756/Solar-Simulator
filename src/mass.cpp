#include "mass.h"

#include <cmath>
#include <vector>

#include "constants.h"
#include "globals.h"

namespace SolarSim {

void Mass::init() {
    // Vector of vertices
    std::vector<float> vertices;

    // (x, y, z) of the origin of the circle
    vertices.push_back(static_cast<float>(x));
    vertices.push_back(static_cast<float>(y));
    vertices.push_back(0.0f);

    // Create a triangle then rotate and make another to make a circle esque shape
    for (int i = 0; i <= numOfVertices; ++i) {
        float angle = 2.0f * static_cast<float>(M_PI) * i / numOfVertices;
        vertices.push_back(radius * std::cos(angle) + static_cast<float>(x));
        vertices.push_back(radius * std::sin(angle) + static_cast<float>(y));
        vertices.push_back(0.0f);
    }

    // Create and assign VAO and VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind VAO and VBO, of and the buffer data
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    // Send the vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void Mass::updateVertices() {
    std::vector<float> vertices;

    // Convert physical position (in meters) to OpenGL coordinates
    // Subtract camera position, so that moving the camera left will move masses to the right
    float drawX = static_cast<float>((x - camX) * screenScale);
    float drawY = static_cast<float>((y - camY) * screenScale);
    float drawRadius = static_cast<float>(radius * screenScale);

    // Center of the mass
    vertices.push_back(drawX);
    vertices.push_back(drawY);
    vertices.push_back(0.0f);

    // Generate circle vertices around the center
    for (int i = 0; i <= numOfVertices; ++i) {
        float angle = 2.0f * static_cast<float>(M_PI) * i / numOfVertices;
        vertices.push_back(drawRadius * std::cos(angle) + drawX);
        vertices.push_back(drawRadius * std::sin(angle) + drawY);
        vertices.push_back(0.0f);
    }

    // Update VBO with the new vertices
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
}

void Mass::draw(unsigned int shader) {
    glUseProgram(shader);

    // Custom Color
    int colorLoc = glGetUniformLocation(shader, "uColor");
    glUniform3f(colorLoc, r, g, b);

    // Bind the Vertex Array Object to it's target and draw the arrays
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, numOfVertices + 2);
}

void Mass::calcAcceleration(double otherMass, double otherX, double otherY) {
    double dx = otherX - x;
    double dy = otherY - y;
    double distSquared = dx * dx + dy * dy;
    double force = Constants::G * otherMass / distSquared;
    double dist = std::sqrt(distSquared);
    ax = force * dx / dist;
    ay = force * dy / dist;
}

void Mass::calcVelocity() {
    vx += ax * timeStepMult;
    vy += ay * timeStepMult;
}

void Mass::calcNewPos() {
    x += (vx * timeStepMult);
    y += (vy * timeStepMult);
}

bool checkCollision(const Mass& m1, const Mass& m2) {
    double dx = m1.x - m2.x;
    double dy = m1.y - m2.y;

    double distSq = dx * dx + dy * dy;
    double minDist = m1.radius + m2.radius;

    return distSq < (minDist * minDist);
}

void resolveCollision(Mass& m1, Mass& m2) {
    double dx = m1.x - m2.x;
    double dy = m1.y - m2.y;
    double dist = std::sqrt(dx * dx + dy * dy);

    if (dist == 0.0) return; // avoid divide by zero

    // Normal vector
    double nx = dx / dist;
    double ny = dy / dist;

    // Relative velocity
    double vx = m1.vx - m2.vx;
    double vy = m1.vy - m2.vy;

    // Relative velocity in terms of normal direction
    double relVel = vx * nx + vy * ny;

    // Only resolve if they are moving toward each other
    if (relVel > 0) return;

    // Elastic collision impulse
    double e = 1.0; // coefficient of restitution (1 = perfectly elastic. adjust as you want)
    double j = -(1 + e) * relVel / (1 / m1.mass + 1 / m2.mass);

    // Apply impulse
    double impulseX = j * nx;
    double impulseY = j * ny;

    // Change velocities based on elastic collision
    m1.vx += impulseX / m1.mass;
    m1.vy += impulseY / m1.mass;
    m2.vx -= impulseX / m2.mass;
    m2.vy -= impulseY / m2.mass;

    // Push them apart slightly so they don’t sink into each other
    double overlap = (m1.radius + m2.radius) - dist;
    m1.x += (overlap / 2) * nx;
    m1.y += (overlap / 2) * ny;
    m2.x -= (overlap / 2) * nx;
    m2.y -= (overlap / 2) * ny;
}

void mergeMasses(Mass& m1, Mass& m2) {
    // conserve momentum
    double totalMass = m1.mass + m2.mass;
    m1.vx = (m1.vx * m1.mass + m2.vx * m2.mass) / totalMass;
    m1.vy = (m1.vy * m1.mass + m2.vy * m2.mass) / totalMass;

    // Set the new mass and radius based on the 2 masses total mass and approximate volume
    m1.mass = totalMass;
    m1.radius = std::cbrt(std::pow(m1.radius, 3) + std::pow(m2.radius, 3)); // approximate volume merge

    m2.mass = 0; // mark m2 as dead :(
}

} // namespace SolarSim
