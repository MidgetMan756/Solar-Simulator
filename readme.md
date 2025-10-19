# SolarSim

A real-time 2D solar system simulation using OpenGL and C++.

Simulate gravitational interactions between celestial bodies such as the Sun, Earth, and Moon. Users can add new masses, pan the camera, and zoom in/out to explore the system.

---

## Features

- Simulates Newtonian gravity between multiple masses
- Elastic collisions or merging of masses
- Real-time visualization using OpenGL
- Mouse controls:
  - **Left-click & drag:** create a new mass with initial velocity
  - **Left-Click on mass:** Displays mass info to terminal
  - **Right-click:** cycle through mass types (Moon, Earth, Sun)
  - **Middle-click & drag:** pan the camera
  - **Scroll wheel:** zoom in/out
- Real-time simulation time display in days, hours, and minutes

---

## Dependencies

**GLFW** - [GLFW](https://www.glfw.org/) — Window and input handling  
**GLAD** - [GLAD](https://glad.dav1d.de/) — OpenGL function loader  
**OpenGL** - OpenGL 3.3 Core Profile  

Ensure these are installed on your system before building.

---

## Project Structure
SolarSim/
├── build/ # Compiled binary
│ └── SolarSim
├── include/ # Header files
│ ├── glad/
│ │ └── glad.h
│ └── KHR/
│ └── khrplatform.h
├── src/ # Source files
│ ├── glad.c
│ └── main.cpp
├── .gitattributes
├── makefile
└── README.md