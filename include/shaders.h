// shaders.h
// Compiled-at-runtime GLSL shader sources.
#pragma once

namespace SolarSim::Shaders {

// Please for all that is holy: DO NOT TOUCH THESE
inline constexpr char vertexShader[] = R"(#version 330 core
layout (location = 0) in vec3 aPos;
void main()
{
   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
})";

inline constexpr char fragmentShader[] = R"(#version 330 core
out vec4 FragColor;
uniform vec3 uColor; // RGB color passed from CPU
void main()
{
    FragColor = vec4(uColor, 1.0);
})";

inline constexpr char textVertexShader[] = R"(#version 330 core
layout (location = 0) in vec2 aPos;
uniform vec2 uScreenSize;
void main()
{
    vec2 ndc = (aPos / uScreenSize) * 2.0 - 1.0;
    ndc.y = -ndc.y;
    gl_Position = vec4(ndc, 0.0, 1.0);
})";

inline constexpr char textFragmentShader[] = R"(#version 330 core
out vec4 FragColor;
uniform vec3 uTextColor;
void main()
{
    FragColor = vec4(uTextColor, 1.0);
})";

} // namespace SolarSim::Shaders
