#include "input.h"

#include <cmath>
#include <iostream>
#include <sstream>

#include "constants.h"
#include "globals.h"
#include "mass.h"
#include "rendering.h"
#include "utils.h"

namespace SolarSim {

int processInput(GLFWwindow* window) {
    // If the escape key was pressed shut down the window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        clearOverlayText();
        glfwSetWindowShouldClose(window, true);

    // If the left mouse button is down and the mouse button isn't already down get the starting pos of the cursor
    } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !isLeftMouseButtonDown) {
        isLeftMouseButtonDown = true;
        glfwGetCursorPos(window, &startxpos, &startypos);

        double worldScreenX, worldScreenY;

        // Get width of the screen
        int fbWidth, fbHeight;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

        // Convert Mouse pos to world pos
        screenToWorld(startxpos, startypos, fbWidth, fbHeight, worldScreenX, worldScreenY);

        // For each mass check if the mouse pos is less that the radius of the mass
        for (size_t i = 0; i < massesVector.size(); ++i) {
            double mouseMassDist = std::sqrt(std::pow(worldScreenX - massesVector[i].x, 2) +
                                             std::pow(worldScreenY - massesVector[i].y, 2));
            if (mouseMassDist <= massesVector[i].radius) {

                clickedExistingMass = true;
                selectedMassIndex = static_cast<int>(i);
                isCameraFollowMass = true;

                // If clicked mass has no name assign it a name
                if (massesVector[i].name.empty()) {
                    massesVector[i].name = "[UNKNOWN]";
                }

                std::ostringstream overlay;
                overlay << "Name: " << massesVector[i].name
                        << "\nMass: " << formatScientific(massesVector[i].mass) << " kg"
                        << "\nRadius: " << formatScientific(massesVector[i].radius) << " m";
                updateOverlayText(overlay.str());
                isLeftMouseButtonDown = false;
            }
        }

    // If the left mouse button isn't down and wasn't already up get the pos of the cursor
    } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && isLeftMouseButtonDown) {
        isLeftMouseButtonDown = false;

        if (clickedExistingMass) {
            clickedExistingMass = false;
            return 0;
        }

        double endx, endy;
        glfwGetCursorPos(window, &endx, &endy);

        // framebuffer size (use framebuffer for pixel-accurate mapping)
        int fbw, fbh;
        glfwGetFramebufferSize(window, &fbw, &fbh);

        // convert both start and end to world, honoring current zoom (screenScale)
        double startWX, startWY, endWX, endWY;
        screenToWorld(startxpos, startypos, fbw, fbh, startWX, startWY);
        screenToWorld(endx, endy, fbw, fbh, endWX, endWY);

        // pick mass archetype
        double min = 0.25;
        double max = 2.25;

        double massMult = Constants::earthMass;
        double radiusMult = Constants::earthRadius;
        float r = 0.75f, g = 0.75f, b = 0.75f;
        switch (massType) {
            case 0:
                r = 0.75f; g = 0.75f; b = 0.75f;
                massMult = Constants::moonMass;
                radiusMult = Constants::moonRadius;
                break;
            case 1:
                r = 0.0f; g = 0.0f; b = 1.0f;
                massMult = Constants::earthMass;
                radiusMult = Constants::earthRadius;
                break;
            case 2:
                r = 0.75f; g = 0.75f; b = 0.0f;
                massMult = Constants::sunMass;
                radiusMult = Constants::sunRadius;
                break;
        }

        float random = randomFloat(static_cast<float>(min), static_cast<float>(max));

        // Create new mass
        Mass temp;
        temp.r = r; temp.g = g; temp.b = b;

        temp.name = getRandomBodyName();

        // place exactly where you clicked (in world units)
        temp.x = startWX;
        temp.y = startWY;

        // give it velocity from the drag vector
        temp.vx = (endWX - startWX) / (timeStepMult * 10);
        temp.vy = (endWY - startWY) / (timeStepMult * 10);

        // Create the new objects mass and radius
        temp.mass   = static_cast<float>(massMult * random);
        temp.radius = static_cast<float>(radiusMult * random);

        // Initialize the new mass and add it to the vector of masses
        temp.init();
        massesVector.push_back(temp);
        clearOverlayText();

    // Right mouse is pressed
    } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && !isRightMouseButtonDown) {
        isRightMouseButtonDown = true;

        // Update the new mass type
        massType++;
        massType = massType % 3;

    // Right mouse is released
    } else if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE && isRightMouseButtonDown) {
        isRightMouseButtonDown = false;

    // Middle mouse is pressed
    } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
        if (!isMiddleMouseButtonDown) {
            // Camera is no longer following mass
            isCameraFollowMass = false;

            // Get the mouse's starting position
            isMiddleMouseButtonDown = true;
            glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
        } else {

            // Mouse is being dragged: pan camera
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            // Get width of the screen
            int fbWidth, fbHeight;
            glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

            // Convert last frame and current frame to world coordinates
            double worldPrevX, worldPrevY, worldNowX, worldNowY;
            screenToWorld(lastMouseX, lastMouseY, fbWidth, fbHeight, worldPrevX, worldPrevY);
            screenToWorld(mouseX, mouseY, fbWidth, fbHeight, worldNowX, worldNowY);

            // Update camera offset by the **drag delta**
            camX += worldPrevX - worldNowX;
            camY += worldPrevY - worldNowY;

            // Update last mouse position for next frame
            lastMouseX = mouseX;
            lastMouseY = mouseY;
        }

    // Middle mouse is released
    } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_RELEASE) {
        isMiddleMouseButtonDown = false;
    }

    return 0;
}

void scroll_callback(GLFWwindow* window, double /*xoffset*/, double yoffset) {
    // Only zoom if a control is being pressed
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) {

        // Increase or decrease the zoom factor by the scroll wheel
        zoomFactor += static_cast<float>(yoffset) * 0.005f;
        if (zoomFactor < 0.005f) zoomFactor = 0.005f; // prevent negative zoom
        screenScale = 1.0 / (Constants::earthMoonDistance * 2) * zoomFactor;
    }

    // Keeping the rest commented out for debugging parity with the original file
    /*
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {

        timeStepMult += static_cast<float>(yoffset) * 10.0f;
        if (timeStepMult < 0) timeStepMult = 0;
        if (timeStepMult > 3000) timeStepMult = 3000;

        std::cout << "\033[6;1H\33[KtimeStepMult " << timeStepMult;
    }
    */
}

} // namespace SolarSim
