#include "config.hpp"
#include "drone.hpp"
#include "zmq_draft.hpp"
#include "filter.hpp"

#include <cstring>
#include <cstdlib>
#include <iostream>
#include <string>

drone::Payload get_msg(zmq::socket_t& dish) {
    zmq::message_t msg;
            
            //creates payload object and places the msg.data() into the payload
            drone::Payload payload{};

            if (!dish.recv(msg, zmq::recv_flags::none)) {
                payload.pose.orientation.w = NAN;
            }

            if (msg.size() != sizeof(drone::Payload)) {
                std::cerr << "recv: unexpected payload size " << msg.size() << '\n';
                payload.pose.orientation.w = NAN;
            }

            if(payload.pose.orientation.w != NAN) std::memcpy(&payload, msg.data(), sizeof(payload)); 
            return payload;
}

int main() {
    try {
        zmq::context_t ctx{1};
        zmq::socket_t dish{ctx, zmq::socket_type::dish};

        dish.bind(std::string{kwx_auto::kEndpoint});
        dish.join(std::string{kwx_auto::kGroup}.c_str());

        std::cout << "receiver: listening on " << kwx_auto::kEndpoint
                  << " group='" << kwx_auto::kGroup << "'\n";

        //TODO: MovingAverage is a class from the filter header
        filter::MovingAverage avg;

        while (true) {
            drone::Payload payload = get_msg(dish);
            if(payload.pose.orientation.w == NAN) continue;
            
            //TODO: implement filtering for variable filtered_z
            double filtered_z = avg.update(payload.pose.position.z);

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
