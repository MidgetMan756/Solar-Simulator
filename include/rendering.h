// rendering.h
// Overlay and HUD rendering utilities.
#pragma once

#include <string>

namespace SolarSim {

void initTextRenderer();
void updateOverlayText(const std::string& text);
void clearOverlayText();
void renderOverlayText();

} // namespace SolarSim
