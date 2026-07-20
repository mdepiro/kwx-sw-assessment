#pragma once

#include "pose.hpp"

#include <cmath>

namespace drone {

/// 0: idle, 1: launching, 2: launched, 3: landing, 4: landed
enum class State {
    Idle = 0,
    Launching = 1,
    Launched = 2,
    Landing = 3,
    Landed = 4,
};

inline State DRONE_STATE{State::Idle};

inline const Pose INITIAL_POSE {
    .position = {0.0, 0.0, 0.0},
    .orientation = {1.0, 0.0, 0.0, 0.0},
};

inline bool poses_equal(const Pose& a, const Pose& b) {
    constexpr double eps = 0.05; //threshold of 5 cm
    return std::abs(a.position.z - b.position.z) < eps;
}

/// Climb from INITIAL_POSE toward `target` by +0.1 m in z each step
/// until the output pose equals the input target.
[[nodiscard]] inline Pose launch_trajectory(const Pose& target) {
    Pose output = INITIAL_POSE;
    output.orientation = target.orientation;
    output.position.x = target.position.x;
    output.position.y = target.position.y;

    for (; !poses_equal(output, target); output.position.z += 0.1) {
        if (output.position.z >= target.position.z) {
            output.position.z -= 0.1;
            break;
        }
    }

    return output;
}

}  // namespace drone
