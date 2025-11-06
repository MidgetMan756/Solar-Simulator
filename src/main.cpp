#include <iostream>
#include <string>
#include <cmath>
#include <random>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

// Please for all that is holy: DO NOT TOUCH THESE
// Shaders for masses
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec3 uColor; // RGB color passed from CPU\n"
    "void main()\n"
    "{\n"
    "FragColor = vec4(uColor, 1.0);\n"
    "}\0";

// Gravistational Const (G)
const double G = 6.674e-11;

// Astronomical values in metric (Meters, m/s, kgs)
const double earthMass = 5.97e24;
const double earthRadius = 6371000;
const double moonMass = 7.35e22;
const double moonRadius = 1740000;
const double sunMass = 1.99e30;
const double sunRadius = 6.957e9;
const double earthMoonDistance = 384400000;
const double sunEarthDistance = 149597870700;
const double moonTanVelocity = 1018.5;
const double earthTanVelocity = 29783;

// How many minutes pass per second
double timeStepMult = 600;

// Camera zoom relative to moons orbital distance
double zoomFactor = 0.9;

// Camera center for panning
double camX = 0.0, camY = 0.0;

// Zoom out to fit everything
double screenScale = 1.0 / (earthMoonDistance * 2) * zoomFactor;  // fit 90% of the screen width at original value

// Updates as mouse is pressed and released
bool isLeftMouseButtonDown = false;
bool isRightMouseButtonDown = false;
bool isMiddleMouseButtonDown = false;

// 0 for moon, 1 for planet, 2 for sun
int massType = 0;

// Start (x,y) pos for the mouse click
double startxpos, startypos;

// Where the mouse was (x,y) in the last frame
double lastMouseX = 0.0, lastMouseY = 0.0;

// GLFW global
GLFWwindow* window;
unsigned int shaderProgram;
unsigned int textShaderProgram;

// Create the class and vector of masses
class Mass;
std::vector<Mass> massesVector;

// is the camera following a mass
bool isCameraFollowMass = false;

// is the user clicking a mass
bool clickedExistingMass = false;
int selectedMassIndex = -1;  // Also fixes follow camera issue

// Vector of mass names
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

// Create functions
double average(const std::vector<double>& v);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
float randomFloat(float min, float max);
int processInput(GLFWwindow* window);
bool checkCollision(const Mass& m1, const Mass& m2);
void resolveCollision(Mass& m1, Mass& m2);
void mergeMasses(Mass& m1, Mass& m2);
void initGLFWnShit();
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void screenToNDC(double sx, double sy, int width, int height,
                        double& nx, double& ny);
void ndcToWorld(double nx, double ny, double& wx, double& wy);
void screenToWorld(double sx, double sy, int width, int height,
                          double& wx, double& wy);
std::string getRandomBodyName();

// Class for each solar mass
class Mass {
    public:

        // BTW this is almost never used, but it could be fun if I could
        // figure out how THE FUCK to get text to work within GLFW
        // Mass name
        std::string name;

        // Mass mass and radius
        float mass;
        float radius;
        
        // (x,y) pos (x,y) velocity (x,y) acceleration
        double x = toOpenGL(0.0f), y = toOpenGL(0.0f);
        double vx = 0.0f, vy = 0.0f;
        double ax = 0.0f, ay = 0.0f;

        // RGB color values
        float r, g, b;

        // Vertex Array Object and Vertex Buffer Object
        unsigned int VAO, VBO;
        
        // number of sides, because circles don't exist, but 360 sided polygons are close enough
        int numOfVertices = 360;

        // Convert astronomical position to OpenGL position (-1,1)
        float toOpenGL(double meters) {
            return (float)(meters * screenScale);
        }

        // Initialize mass
        void init() {
            // Vector of vertices
            std::vector<float> vertices;

            // (x, y, z) of the origin of the circle
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(0.0f);

            // Create a triangle then rotate and make another to make a circle esque shape
            for (int i = 0; i <= numOfVertices; ++i) {
                float angle = 2.0f * M_PI * i / numOfVertices;
                vertices.push_back(radius * cos(angle) + x);
                vertices.push_back(radius * sin(angle) + y);
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

        // Update the vertices based on new location
        void updateVertices() {
            std::vector<float> vertices;

            // Convert physical position (in meters) to OpenGL coordinates
            // Subtact camera position, so that moving the camera left will move masses to the right
            float drawX = (float)((x - camX) * screenScale);
            float drawY = (float)((y - camY) * screenScale);
            float drawRadius = (float)(radius * screenScale);

            // Center of the mass
            vertices.push_back(drawX);
            vertices.push_back(drawY);
            vertices.push_back(0.0f);

            // Generate circle vertices around the center
            for (int i = 0; i <= numOfVertices; ++i) {
                float angle = 2.0f * M_PI * i / numOfVertices;
                vertices.push_back(drawRadius * cos(angle) + drawX);
                vertices.push_back(drawRadius * sin(angle) + drawY);
                vertices.push_back(0.0f);
            }

            // Update VBO with the new vertices
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
        }


        // Draw the circle
        void draw(unsigned int shaderProgram) {
            glUseProgram(shaderProgram);

            // Custom Color
            int colorLoc = glGetUniformLocation(shaderProgram, "uColor");
            glUniform3f(colorLoc, r, g, b);

            // Bind the Vertex Array Object to it's target and draw the arrays
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLE_FAN, 0, numOfVertices + 2);
        }

        // Calculate the acceleration using GM/r^2
        void calcAcceleration(double otherMass, double otherX, double otherY) {
            double dx = otherX - x;
            double dy = otherY - y;
            double distSquared = dx * dx + dy * dy;
            double force = G * otherMass / distSquared;
            double dist = sqrt(distSquared);
            ax = force * dx / dist;
            ay = force * dy / dist;
        }

        // timeStepMult used to speed up time
        // Add acceleration to the velocity
        void calcVelocity() {
            vx += ax * timeStepMult;
            vy += ay * timeStepMult;
        }

        // Calculate the new position using velocity
        void calcNewPos() {
            x += (vx * timeStepMult);
            y += (vy * timeStepMult);
        }

};

// Main loop
int main() {

    int frame = 0;

    // set up GLFW window and all the other things
    initGLFWnShit();
    glfwSetScrollCallback(window, scroll_callback);

    // Create an instance of mass based off the sun
    Mass sun;
    sun.r = 0.75f, sun.g = 0.75f, sun.b = 0.0f;
    sun.name = "Sun";
    sun.mass = sunMass;
    sun.radius = sunRadius * 20;
    sun.x = -sunEarthDistance;
    sun.vx = 0;
    sun.init();

    // Add sun to the vector of masses
    //massesVector.push_back(sun);

    // Create an instance of mass based off the earth
    Mass earth;
    earth.r = 0.0f, earth.g = 0.0f, earth.b = 1.0f;
    earth.name = "Earth";
    earth.mass = earthMass;
    earth.radius = earthRadius;
    earth.x = 0;
    earth.vy = 0;
    earth.init();

    // Add earth to the vector of masses
    massesVector.push_back(earth);

    // Create an insance of mass based off the moon
    Mass moon;
    moon.r = 1.5f, moon.g = 1.5f, moon.b = 1.5f;
    moon.name = "Moon";
    moon.mass = moonMass;
    moon.radius = moonRadius;
    moon.x = earthMoonDistance;
    moon.vy = moonTanVelocity;
    moon.init();

    // Add moon to the vector masses
    massesVector.push_back(moon);

    // Destroy stuff ONLY when told
    while(!glfwWindowShouldClose(window)) {
        // Check keypresses
        processInput(window);

        // Render loop

        // If the camera should follow a mass set cam x and y to match the masses x and y
        if (isCameraFollowMass && selectedMassIndex >= 0 && selectedMassIndex < static_cast<int>(massesVector.size())) {
            camX = massesVector[selectedMassIndex].x;
            camY = massesVector[selectedMassIndex].y;
        }

        // Set bg color
        glClearColor(0.025f, 0.005f, 0.075f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Count ammount of frames have passed
        frame++;

        // How many seconds have pasesed
        double totalSimSeconds = frame * timeStepMult;

        // Calculate days, hours, minutes, and seconds
        int totalSeconds = static_cast<int>(totalSimSeconds);
        int days = totalSeconds / 86400;
        int hours = (totalSeconds % 86400) / 3600;
        int minutes = (totalSeconds % 3600) / 60;

        // Display time elapsed and move the mouse cursor back to first line to print on one line
        std::cout << "\033[HSimulated Time: " << days << "d " << hours << "h " << minutes << "m\n";

        // For each mass calculate the gravitational pull every
        // other mass applies to itself then average it and set it to the current mass
        for (size_t i = 0; i < massesVector.size(); i++) {
            // Get individual masses
            Mass& m1 = massesVector[i];

            // Create vector to store acceleration vectors
            std::vector<double> m1axVector;
            std::vector<double> m1ayVector;

            // Reset acceleration
            m1.ax = 0, m1.ay = 0;

            // For every other mass
            for (size_t j = 0; j < massesVector.size(); j++) {
                if (i==j) continue; // Don't pull yourself
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

        // Check for collision and either bounce the objects or merge the masses
        // (swap with line above or below the or is commented in order to swap between merge and bounce)
        for (size_t i = 0; i < massesVector.size(); i++) {
            for (size_t j = i+1; j < massesVector.size(); j++) {
                if (checkCollision(massesVector[i], massesVector[j])) {
                    resolveCollision(massesVector[i], massesVector[j]); 
                    // OR 
                    // mergeMasses(massesVector[i], massesVector[j]);
                }
            }
        }

        // Only ran if masses merge, checks if mass is 0 then deleates it so it's not used
        // in calculating acceleration of other masses
        massesVector.erase(
            std::remove_if(massesVector.begin(), massesVector.end(),
                        [](const Mass& m){ return m.mass <= 0; }),
            massesVector.end()
        );
 
        // Use shader program and bind VAO
        glUseProgram(shaderProgram);

        // Check if the window should close
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    // destroy the window and shut everything down when the programs over
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

// function to average all items in a vector. Used to average masses velocity vector
double average(const std::vector<double>& v) {
    if (v.empty()) return 0; // if the vector is empty
    double sum = std::accumulate(v.begin(), v.end(), 0.0); // get sum
    return sum / v.size(); // return sum / size
}

// each frame run this to modify the size of the window's viewport
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {

        // Changes the window's viewport size
        glViewport(0, 0, width, height);
}

// randomly generate a float
float randomFloat(float min, float max) {
    static std::random_device rd;  // seed
    static std::mt19937 gen(rd()); // mersenne twister
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}

// Process keyboard & mouse input
int processInput(GLFWwindow* window) {
    
    // If the escape key was pressed shut down the window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
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
        for (long unsigned int i = 0; i < massesVector.size(); i++) {
            double mouseMassDist = std::pow(std::pow((worldScreenX - massesVector[i].x), 2) + std::pow((worldScreenY - massesVector[i].y), 2), 0.5f);
            if (mouseMassDist <= massesVector[i].radius) {

                clickedExistingMass = true;
                selectedMassIndex = i;
                isCameraFollowMass = true;

                // If clicked mass has no name assign it a name
                if (massesVector[i].name == "") {
                    massesVector[i].name = "[UNKNOWN]";
                }

                // Print out mass info and set mouse isLeftMouseButtonDown to false so it doesn't create a new mass
                std::cout << "\033[2;1H\33[KName: " << massesVector[i].name << "\n\33[KMass: " <<  massesVector[i].mass << " Kilograms\n\33[KRadius: " << massesVector[i].radius << " Meters";
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
        // end cursor
        double endx, endy;
        glfwGetCursorPos(window, &endx, &endy);

        // framebuffer size (use framebuffer for pixel-accurate mapping)
        int fbw, fbh;
        glfwGetFramebufferSize(window, &fbw, &fbh);

        // convert both start and end to world, honoring current zoom (screenScale)
        double startWX, startWY, endWX, endWY;
        screenToWorld(startxpos, startypos, fbw, fbh, startWX, startWY);
        screenToWorld(endx,      endy,      fbw, fbh, endWX,   endWY);

        // pick mass archetype
        double min = 0.25;
        double max = 2.25;

        double massMult = earthMass, radiusMult = earthRadius;
        float r=0.75f, g=0.75f, b=0.75f;
        switch (massType) {
            case 0: r=0.75f; g=0.75f; b=0.75f; massMult = moonMass;  radiusMult = moonRadius;  break;
            case 1: r=0.0f;  g=0.0f;  b=1.0f;  massMult = earthMass; radiusMult = earthRadius; break;
            case 2: r=0.75f; g=0.75f; b=0.0f;  massMult = sunMass;   radiusMult = sunRadius;   break;
        }

        float random = randomFloat((float)min, (float)max);

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
        temp.mass   = massMult   * random;
        temp.radius = radiusMult * random;

        // Initialize the new mass and add it to the vector of masses
        temp.init();
        massesVector.push_back(temp);
    
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

            // Get the mouses starting position
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

// Check if the masses are colliding
bool checkCollision(const Mass& m1, const Mass& m2) {
    // Get the x dist and y dist
    double dx = m1.x - m2.x;
    double dy = m1.y - m2.y;

    // Calc the squared distance
    double distSq = dx*dx + dy*dy;
    
    // Closest the two masses can be without existing in the same space
    double minDist = m1.radius + m2.radius;

    // if (distSq < (minDist * minDist)){
    //     std::cout << m1.name << " collided with " << m2.name;
    // }   
    // Return true if the distance is less than the least possible distance
    return distSq < (minDist * minDist);
}

// Bounce masses
void resolveCollision(Mass& m1, Mass& m2) {

    // Calculate the distance between the two masses
    double dx = m1.x - m2.x;
    double dy = m1.y - m2.y;
    double dist = std::sqrt(dx*dx + dy*dy);

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
    double j = -(1 + e) * relVel / (1/m1.mass + 1/m2.mass);

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
    m1.x += (overlap/2) * nx;
    m1.y += (overlap/2) * ny;
    m2.x -= (overlap/2) * nx;
    m2.y -= (overlap/2) * ny;
}

// Merge masses
void mergeMasses(Mass& m1, Mass& m2) {
    // conserve momentum
    double totalMass = m1.mass + m2.mass;
    m1.vx = (m1.vx * m1.mass + m2.vx * m2.mass) / totalMass;
    m1.vy = (m1.vy * m1.mass + m2.vy * m2.mass) / totalMass;

    // Set the new mass and radius based on the 2 masses total mass and approximate volume
    m1.mass = totalMass;
    m1.radius = std::cbrt(std::pow(m1.radius,3) + std::pow(m2.radius,3)); // approximate volume merge

    m2.mass = 0; // mark m2 as dead :(
}

// stuff to initialize GLFW and other shit
void initGLFWnShit(){
    // Init glfw
    glfwInit();

    // Let glfw know what we're using
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create 1000x1000 window
    window = glfwCreateWindow(1000, 1000, "Solar Sim", NULL, NULL);

    // If window isn't made
    if (window == NULL) {
        std::cerr << "Trouble making window";
        glfwTerminate();
    }

    // Set current window context
    glfwMakeContextCurrent(window);

    // set up glad for colors and stuff
    gladLoadGL();

    // set up the current frame
    glViewport(0, 0, 1000, 1000);

    // Allows window to be resized
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Create dark blue background
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Create Vertex Buffer Object (VBO) and save it's ID
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // Create Vertex Array Object (VAO) and save it's ID
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    // Bind Vertex Array Object
    glBindVertexArray(VAO);

    // Bind the Vertex Buffer Object to target
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // For masses
    // Create a shader object and save it's ID
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // Attach the source code and compile
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Create a shader object and save it's ID
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Attach the source code and compile
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Create a program and save it's ID
    shaderProgram = glCreateProgram();

    // Attach shaders to the program and link them
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Use program
    glUseProgram(shaderProgram);

    // Delete shaders to free mem
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Tell OpenGL how to interpret the vertices and enable it
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

// Function to run when the scrollwheel is scrolled
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {

    // Only zoom if a control is being pressed
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
    glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) {

        // Increate or decrease the zoom factor by the scroll wheels scroll
        zoomFactor += (float)yoffset * 0.005f;
        if (zoomFactor < 0.005f) zoomFactor = 0.005; // prevent negative zoom
        screenScale = 1.0 / (384400000.0 * 2) * zoomFactor;
    }

    // This doesn't work properly, mostly for debugging

    // if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
    // glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {

    //     // Increate or decrease the zoom factor by the scroll wheels scroll
    //     timeStepMult += (float)yoffset * 10.0f;
    //     if (timeStepMult < 0) timeStepMult = 0; // prevent negative time
    //     if (timeStepMult > 3000) timeStepMult = 3000; // prevents unrealistic physics due to being too fast

    //     std::cout << "\033[6;1H\33[KtimeStepMult " << timeStepMult;

    // }
}

// Convert GLFW coords to NDC
void screenToNDC(double sx, double sy, int width, int height,
                        double& nx, double& ny) {
    nx = (sx / width) * 2.0 - 1.0;
    ny = 1.0 - (sy / height) * 2.0; // flip Y (GLFW top-left -> NDC top)
}

// Convert NDC coords to World
void ndcToWorld(double nx, double ny, double& wx, double& wy) {
    // inverse of world->NDC: draw = world * screenScale
    wx = nx / screenScale + camX;
    wy = ny / screenScale + camY;
}

// Convert GLFW coords to World
void screenToWorld(double sx, double sy, int width, int height,
                          double& wx, double& wy) {
    double nx, ny;
    screenToNDC(sx, sy, width, height, nx, ny);
    ndcToWorld(nx, ny, wx, wy);
}

// Function to return a random name from the list
std::string getRandomBodyName() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, celestialBodies.size() - 1);

    return celestialBodies[dist(gen)];
}