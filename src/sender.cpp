#include "config.hpp"
#include "zmq_draft.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

int main() {
    try {
        zmq::context_t ctx{1};
        zmq::socket_t radio{ctx, zmq::socket_type::radio};

        // RADIO connects; DISH binds (ZeroMQ UDP pub/sub pattern).
        radio.connect(std::string{sw_assessment::kEndpoint});

        std::cout << "sender: publishing to " << sw_assessment::kEndpoint
                  << " group='" << sw_assessment::kGroup << "'\n";

        unsigned int seq = 0;
        while (true) {
            const std::string payload = "hello " + std::to_string(seq++);

            zmq::message_t msg{payload.data(), payload.size()};
            msg.set_group(std::string{sw_assessment::kGroup}.c_str());

            radio.send(msg, zmq::send_flags::none);
            std::cout << "sent: " << payload << '\n';

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
