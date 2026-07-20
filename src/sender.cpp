#include "config.hpp"
#include "zmq_draft.hpp"
#include "pose.hpp"
#include "drone.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

using namespace drone;
using namespace pose;

Pose DRONE_SET_POSE = {
    .position = {0.0, 0.0, 5.0},
    .orientation = {1.0, 0.0, 0.0, 0.0}
}

//0: idle, 1: launching, 2: launched, 3: landing, 4: landed
State DRONE_STATE = State::Idle; 

int main() {
    try {
        zmq::context_t ctx{1};
        zmq::socket_t radio{ctx, zmq::socket_type::radio};

        // RADIO connects; DISH binds (ZeroMQ UDP pub/sub pattern).
        radio.connect(std::string{kwx_auto::kEndpoint});

        std::cout << "sender: publishing to " << kwx_auto::kEndpoint
                  << " group='" << kwx_auto::kGroup << "'\n";

        //unsigned int seq = 0;
        while (true) {
            std::string payload = std::to_string(static_cast<int>(DRONE_STATE));

            zmq::message_t msg{payload.data(), payload.size()};
            msg.set_group(std::string{kwx_auto::kGroup}.c_str());

            radio.send(msg, zmq::send_flags::none);
            std::cout << "sent: " << payload << '\n';

            if (DRONE_STATE == State::Idle) {const std::string payload = "LAUNCH";
                Pose DRONE_CURRENT_POSE = {
                    .position = {0.0, 0.0, 0.0},
                    .orientation = {1.0, 0.0, 0.0, 0.0}
                }
                DRONE_STATE = State::Launching;
                continue;
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
