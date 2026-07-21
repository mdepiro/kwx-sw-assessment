#pragma once

#include "pose.hpp"

#include <cmath>
#include <iostream>

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

inline const pose::Pose INITIAL_POSE{
    .position = {0.0, 0.0, 0.0},
    .orientation = {1.0, 0.0, 0.0, 0.0},
};

const double LANDING_THRESHOLD = 0.03; // kill engine at 2 cm from ground

inline bool poses_equal(const pose::Pose& a, const pose::Pose& b) {
    constexpr double eps = 0.05;  // 5 cm
    return std::abs(a.position.z - b.position.z) < eps;
}

/// Climb from INITIAL_POSE toward target by +0.1 m in z each step
/// until the drone has reached the target height.
[[nodiscard]] inline pose::Pose launch_trajectory(pose::Pose& output, const pose::Pose& target) {
    bool acheived_target = poses_equal(output, target);

    output.orientation = target.orientation;
    output.position.x = target.position.x;
    output.position.y = target.position.y;

    while(!acheived_target){
        //std::cout << "traj  " << (output.position.z + 0.1) << '\n';
        output.position.z += 0.1;
        if (output.position.z >= target.position.z) {
            output.position.z -= 0.05;
            break;
        } else {break;}
    }
    if (acheived_target) {DRONE_STATE = State::Launched;}
    return output;
}

[[nodiscard]] inline pose::Pose land_trajectory(pose::Pose& output, const pose::Pose& target) {
    bool acheived_target = poses_equal(output, target);

    output.orientation = target.orientation;
    output.position.x = target.position.x;
    output.position.y = target.position.y;

    while(!acheived_target){
        //std::cout << "traj  " << (output.position.z + 0.1) << '\n';
        output.position.z -= 0.1;
        if (output.position.z <= target.position.z + LANDING_THRESHOLD) {
            output.position.z = INITIAL_POSE.position.z;
            acheived_target = true;
            break;
        } else {break;}
    }
    if (acheived_target) {DRONE_STATE = State::Landed;}
    return output;
}

}  // namespace drone
