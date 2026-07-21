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

const pose::Pose TARGET_POSE{
    .position = {0.0, 0.0, 5.0},
    .orientation = {1.0, 0.0, 0.0, 0.0},
};

} 

void send_msg(drone::State drone_state, pose::Pose curr_pose) {
    const std::string payload =
        std::to_string(static_cast<int>(drone::DRONE_STATE));

        zmq::message_t msg{payload.data(), payload.size()};
        msg.set_group(std::string{kwx_auto::kGroup}.c_str());

        radio.send(msg, zmq::send_flags::none);
        std::cout << "sent: state=" << payload
                << " z=" << current_pose.position.z << '\n';
}

int main() {
    try {
        zmq::context_t ctx{1};
        zmq::socket_t radio{ctx, zmq::socket_type::radio};

        radio.connect(std::string{kwx_auto::kEndpoint});

        std::cout << "sender: publishing to " << kwx_auto::kEndpoint
                  << " group='" << kwx_auto::kGroup << "'\n";

        pose::Pose current_pose = drone::INITIAL_POSE;

        while (true) {
            send_msg(drone::DRONE_STATE, current_pose);
            /* const std::string payload =
                std::to_string(static_cast<int>(drone::DRONE_STATE));

            zmq::message_t msg{payload.data(), payload.size()};
            msg.set_group(std::string{kwx_auto::kGroup}.c_str());

            radio.send(msg, zmq::send_flags::none);
            std::cout << "sent: state=" << payload
                      << " z=" << current_pose.position.z << '\n'; */

            if (drone::DRONE_STATE == drone::State::Idle) {
                //Launch the drone
                drone::DRONE_STATE = drone::State::Launching;
                continue;
            } else if (drone::DRONE_STATE == drone::State::Launching) {
                current_pose = drone::launch_trajectory(TARGET_POSE);
                drone::DRONE_STATE = drone::State::Launched;
            }

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
