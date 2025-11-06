#include <algorithm>
#include <exception>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>

#include "constants.h"
#include "globals.h"
#include "input.h"
#include "mass.h"
#include "rendering.h"
#include "utils.h"
#include "window.h"

using namespace SolarSim;

// Destroy stuff ONLY when told
int main() {
    try {
        initWindow();
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialise SolarSim: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    glfwSetScrollCallback(window, scroll_callback);

    int frame = 0;

    // Create an instance of mass based off the sun
    Mass sun;
    sun.r = 0.75f; sun.g = 0.75f; sun.b = 0.0f;
    sun.name = "Sun";
    sun.mass = static_cast<float>(Constants::sunMass);
    sun.radius = static_cast<float>(Constants::sunRadius * 20);
    sun.x = -Constants::sunEarthDistance;
    sun.vx = 0;
    sun.init();

    // Add sun to the vector of masses
    // massesVector.push_back(sun);

    // Create an instance of mass based off the earth
    Mass earth;
    earth.r = 0.0f; earth.g = 0.0f; earth.b = 1.0f;
    earth.name = "Earth";
    earth.mass = static_cast<float>(Constants::earthMass);
    earth.radius = static_cast<float>(Constants::earthRadius);
    earth.x = 0;
    earth.vy = 0;
    earth.init();

    // Add earth to the vector of masses
    massesVector.push_back(earth);

    // Create an instance of mass based off the moon
    Mass moon;
    moon.r = 1.5f; moon.g = 1.5f; moon.b = 1.5f;
    moon.name = "Moon";
    moon.mass = static_cast<float>(Constants::moonMass);
    moon.radius = static_cast<float>(Constants::moonRadius);
    moon.x = Constants::earthMoonDistance;
    moon.vy = Constants::moonTanVelocity;
    moon.init();

    // Add moon to the vector masses
    massesVector.push_back(moon);

    while (!glfwWindowShouldClose(window)) {
        // Check keypresses
        processInput(window);

        // Render loop

        // If the camera should follow a mass set cam x and y to match the masses x and y
        if (isCameraFollowMass && selectedMassIndex >= 0 &&
            selectedMassIndex < static_cast<int>(massesVector.size())) {
            camX = massesVector[selectedMassIndex].x;
            camY = massesVector[selectedMassIndex].y;
        }

        // Set bg color
        glClearColor(0.025f, 0.005f, 0.075f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Count amount of frames have passed
        frame++;

        // How many seconds have passed
        double totalSimSeconds = frame * timeStepMult;

        // Calculate days, hours, minutes, and seconds
        int totalSeconds = static_cast<int>(totalSimSeconds);
        int days = totalSeconds / 86400;
        int hours = (totalSeconds % 86400) / 3600;
        int minutes = (totalSeconds % 3600) / 60;
        int seconds = totalSeconds % 60;

        std::ostringstream timeStream;
        timeStream << "Sim Time: "
                   << days << "d "
                   << std::setfill('0') << std::setw(2) << hours << "h "
                   << std::setw(2) << minutes << "m "
                   << std::setw(2) << seconds << "s";
        timeOverlayText = timeStream.str();

        // For each mass calculate the gravitational pull every other mass applies to itself then average it
        for (size_t i = 0; i < massesVector.size(); ++i) {
            Mass& m1 = massesVector[i];

            std::vector<double> m1axVector;
            std::vector<double> m1ayVector;

            // Reset acceleration
            m1.ax = 0.0;
            m1.ay = 0.0;

            // For every other mass
            for (size_t j = 0; j < massesVector.size(); ++j) {
                if (i == j) continue; // Don't pull yourself
                Mass& m2 = massesVector[j]; // Get other mass
                m1.calcAcceleration(m2.mass, m2.x, m2.y);

                // Push the acceleration vectors the other mass is having on the first mass to the vector
                m1axVector.push_back(m1.ax);
                m1ayVector.push_back(m1.ay);
            }

            // Average each masses acceleration vectors to get one vector
            m1.ax = average(m1axVector);
            m1.ay = average(m1ayVector);
        }

        // Update each mass
        // (uncomment the 2 lines in the for loop so that the earth doesn't move to simulate earth moon orbit)
        for (Mass& m : massesVector) {
            // if (m.name != "Earth") {
            m.calcVelocity();
            m.calcNewPos();
            // }
            m.updateVertices();
            m.draw(shaderProgram);
        }

        renderOverlayText();

        // Check for collision and either bounce the objects or merge the masses
        // (swap with line above or below the or is commented in order to swap between merge and bounce)
        for (size_t i = 0; i < massesVector.size(); ++i) {
            for (size_t j = i + 1; j < massesVector.size(); ++j) {
                if (checkCollision(massesVector[i], massesVector[j])) {
                    resolveCollision(massesVector[i], massesVector[j]);
                    // OR
                    // mergeMasses(massesVector[i], massesVector[j]);
                }
            }
        }

        // Only ran if masses merge, checks if mass is 0 then deletes it so it's not used
        // in calculating acceleration of other masses
        massesVector.erase(
            std::remove_if(massesVector.begin(), massesVector.end(),
                           [](const Mass& m){ return m.mass <= 0; }),
            massesVector.end()
        );

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    shutdownWindow();
    return EXIT_SUCCESS;
}
