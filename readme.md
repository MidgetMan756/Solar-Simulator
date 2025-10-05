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
  - **Right-click:** cycle through mass types (Moon, Earth, Sun)
  - **Middle-click & drag:** pan the camera
  - **Scroll wheel:** zoom in/out
- Real-time simulation time display in days, hours, and minutes

---

## Dependencies

- **GLFW**: For window management and input  
- **GLAD**: For OpenGL function loading  
- **OpenGL**: Rendering graphics  

Ensure these are installed on your system before building.

---

## Project Structure

