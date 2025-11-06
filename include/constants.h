// constants.h
// Physical constants and astronomical reference values used across the simulation.
#pragma once

namespace SolarSim::Constants {

// Gravitational constant (G)
inline constexpr double G = 6.674e-11;

// Astronomical values in metric units (meters, m/s, kilograms)
inline constexpr double earthMass = 5.97e24;
inline constexpr double earthRadius = 6'371'000;
inline constexpr double moonMass = 7.35e22;
inline constexpr double moonRadius = 1'740'000;
inline constexpr double sunMass = 1.99e30;
inline constexpr double sunRadius = 6.957e9;
inline constexpr double earthMoonDistance = 384'400'000;
inline constexpr double sunEarthDistance = 149'597'870'700;
inline constexpr double moonTanVelocity = 1018.5;
inline constexpr double earthTanVelocity = 29'783;

} // namespace SolarSim::Constants
