#include "config.hpp"
#include "drone.hpp"
#include "zmq_draft.hpp"
#include "filter.hpp"

#include <cstring>
#include <cstdlib>
#include <iostream>
#include <string>

int main() {
    try {
        zmq::context_t ctx{1};
        zmq::socket_t dish{ctx, zmq::socket_type::dish};

        dish.bind(std::string{kwx_auto::kEndpoint});
        dish.join(std::string{kwx_auto::kGroup}.c_str());

        std::cout << "receiver: listening on " << kwx_auto::kEndpoint
                  << " group='" << kwx_auto::kGroup << "'\n";

        filter::MovingAverage z_filter;

        while (true) {
            zmq::message_t msg;
            if (!dish.recv(msg, zmq::recv_flags::none)) {
                continue;
            }

            if (msg.size() != sizeof(drone::Payload)) {
                std::cerr << "recv: unexpected payload size " << msg.size() << '\n';
                continue;
            }
            
            //creates payload object and places the msg.data() into the payload
            drone::Payload payload{};
            std::memcpy(&payload, msg.data(), sizeof(payload)); 
            
            const double filtered_z = z_filter.update(payload.pose.position.z);

            std::cout << "recv: state=" << drone::FlightState.at(payload.state)
                      << " z=" << payload.pose.position.z
                      << " filtered_z=" << filtered_z << '\n';
        }
    } catch (const zmq::error_t& e) {
        std::cerr << "receiver zmq error: " << e.what() << '\n';
        return EXIT_FAILURE;
    } catch (const std::exception& e) {
        std::cerr << "receiver error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }
}
