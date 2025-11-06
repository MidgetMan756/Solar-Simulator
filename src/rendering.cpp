#include "rendering.h"

#include <algorithm>

#define STB_EASY_FONT_IMPLEMENTATION
#include "stb_easy_font.h"

#include "globals.h"
#include "utils.h"

namespace SolarSim {

void initTextRenderer() {
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void updateOverlayText(const std::string& text) {
    overlayText = text;
    showTextOverlay = !overlayText.empty();
}

void clearOverlayText() {
    showTextOverlay = false;
    textVertices.clear();
    textVertexCount = 0;
    overlayText.clear();
}

bool buildTextMesh(const std::string& text, float scale, float offsetX, float offsetY, int& outVertexCount) {
    outVertexCount = 0;
    if (text.empty()) return false;

    size_t bufferSize = std::max<size_t>(1, text.size()) * kEasyFontBytesPerChar;
    textScratchBuffer.resize(bufferSize);

    int quadCount = stb_easy_font_print(0.0f, 0.0f, const_cast<char*>(text.c_str()), nullptr,
                                        textScratchBuffer.data(), static_cast<int>(bufferSize));
    if (quadCount <= 0) return false;

    struct EasyFontVertex {
        float x;
        float y;
        float z;
        unsigned char rgba[4];
    };

    EasyFontVertex* verts = reinterpret_cast<EasyFontVertex*>(textScratchBuffer.data());
    textVertices.clear();
    textVertices.reserve(static_cast<size_t>(quadCount) * 6 * 2);

    for (int i = 0; i < quadCount; ++i) {
        EasyFontVertex* quad = verts + i * 4;
        auto emit = [&](int idx) {
            textVertices.push_back(quad[idx].x * scale + offsetX);
            textVertices.push_back(quad[idx].y * scale + offsetY);
        };

        emit(0); emit(1); emit(2);
        emit(0); emit(2); emit(3);
    }

    outVertexCount = static_cast<int>(textVertices.size() / 2);
    if (outVertexCount == 0) return false;

    glBufferData(GL_ARRAY_BUFFER, textVertices.size() * sizeof(float), textVertices.data(), GL_DYNAMIC_DRAW);
    textVertexCount = outVertexCount;
    return true;
}

void renderOverlayText() {
    textVertexCount = 0;

    int fbWidth = 0, fbHeight = 0;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    if (fbWidth <= 0 || fbHeight <= 0) return;

    glUseProgram(textShaderProgram);
    glUniform2f(textScreenUniform, static_cast<float>(fbWidth), static_cast<float>(fbHeight));
    glUniform3f(textColorUniform, 0.85f, 0.85f, 0.9f);

    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);

    int vertexCount = 0;

    if (!timeOverlayText.empty()) {
        if (buildTextMesh(timeOverlayText, timeOverlayScale, timeOverlayMargin, timeOverlayMargin, vertexCount)) {
            glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        }
    }

    if (showTextOverlay && !overlayText.empty()) {
        float rawWidth = static_cast<float>(stb_easy_font_width(const_cast<char*>(overlayText.c_str())));
        float offsetX = static_cast<float>(fbWidth) - rawWidth * overlayScale - overlayMargin;
        if (offsetX < overlayMargin) offsetX = overlayMargin;
        if (buildTextMesh(overlayText, overlayScale, offsetX, overlayMargin, vertexCount)) {
            glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(shaderProgram);
}

} // namespace SolarSim
