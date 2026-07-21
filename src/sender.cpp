#include "config.hpp"
#include "drone.hpp"
#include "pose.hpp"
#include "zmq_draft.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

namespace {

const int64_t TIMEOUT = 2000; //milliseconds
int64_t elapsed_time = 0;

using TimePoint = std::chrono::steady_clock::time_point;
TimePoint current_time;

auto start_time = std::chrono::steady_clock::now();

const pose::Pose TARGET_POSE{
    .position = {0.0, 0.0, 5.0},
    .orientation = {1.0, 0.0, 0.0, 0.0},
};

pose::GetPose get_pose;
} 

void send_msg(zmq::socket_t& radio, pose::Pose current_pose) {

    const std::string payload =
        std::to_string(static_cast<int>(drone::DRONE_STATE));

        zmq::message_t msg{payload.data(), payload.size()};
        msg.set_group(std::string{kwx_auto::kGroup}.c_str());

        radio.send(msg, zmq::send_flags::none);
        std::cout << "sent: state=" << payload
                << " z=" << current_pose.position.z << '\n';
}

pose::Pose drone_action(drone::State drone_state, pose::Pose current_pose) {
    pose::Pose pose;
    switch(drone_state) {
        case drone::State::Idle:
            pose = current_pose;
            drone::DRONE_STATE = drone::State::Launching;
        break;
        case drone::State::Launching:
            start_time = std::chrono::steady_clock::now();
            pose = drone::launch_trajectory(current_pose, TARGET_POSE); //returns ideal next step in traj.
            pose = get_pose.generate(pose); //get the actual pose
        break;
        case drone::State::Launched:
            current_time = std::chrono::steady_clock::now();
            elapsed_time = 
                std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time)
                .count();
            if (elapsed_time >= TIMEOUT) {
                drone::DRONE_STATE = drone::State::Landing;
            }
            pose = current_pose;
        break;
        case drone::State::Landing:
            pose = drone::land_trajectory(current_pose, drone::INITIAL_POSE);
        break;
        case drone::State::Landed:
            drone::DRONE_STATE = drone::State::Idle;
        break;
    }
    return pose;
}

int main() {
    try {
        zmq::context_t ctx{1};
        zmq::socket_t radio{ctx, zmq::socket_type::radio};

        radio.connect(std::string{kwx_auto::kEndpoint});

        std::cout << "sender: publishing to " << kwx_auto::kEndpoint
                  << " group='" << kwx_auto::kGroup << "'\n";

        pose::Pose current_pose = drone::INITIAL_POSE;
        send_msg(radio, current_pose);

        while (true) {
            //get current pose
            current_pose = get_pose.generate(current_pose);
            //poll the drone for an action
            current_pose = drone_action(drone::DRONE_STATE, current_pose);
            //send data
            send_msg(radio, current_pose);
            //rate control
            std::this_thread::sleep_for(std::chrono::seconds{1});
        }

    } catch (const zmq::error_t& e) {
        std::cerr << "sender zmq error: " << e.what() << '\n';
        return EXIT_FAILURE;
    } catch (const std::exception& e) {
        std::cerr << "sender error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }
}
