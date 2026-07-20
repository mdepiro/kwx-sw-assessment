#pragma once

#include <string_view>

namespace sw_assessment {

// UDP multicast endpoint shared by sender (RADIO) and receiver (DISH).
inline constexpr std::string_view kEndpoint = "udp://239.255.0.1:3456";

// ZeroMQ RADIO/DISH group name (analogous to a PUB/SUB topic).
inline constexpr std::string_view kGroup = "assessment";

}  // namespace sw_assessment
