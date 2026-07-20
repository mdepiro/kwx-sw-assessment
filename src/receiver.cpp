#include "config.hpp"
#include "zmq_draft.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

int main() {
    try {
        zmq::context_t ctx{1};
        zmq::socket_t dish{ctx, zmq::socket_type::dish};

        // DISH binds; RADIO connects (ZeroMQ UDP pub/sub pattern).
        dish.bind(std::string{sw_assessment::kEndpoint});
        dish.join(std::string{sw_assessment::kGroup}.c_str());

        std::cout << "receiver: listening on " << sw_assessment::kEndpoint
                  << " group='" << sw_assessment::kGroup << "'\n";

        while (true) {
            zmq::message_t msg;
            if (!dish.recv(msg, zmq::recv_flags::none)) {
                continue;
            }

            const std::string payload{static_cast<char*>(msg.data()), msg.size()};
            std::cout << "recv group='" << msg.group() << "' payload='" << payload
                      << "'\n";
        }
    } catch (const zmq::error_t& e) {
        std::cerr << "receiver zmq error: " << e.what() << '\n';
        return EXIT_FAILURE;
    } catch (const std::exception& e) {
        std::cerr << "receiver error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }
}
