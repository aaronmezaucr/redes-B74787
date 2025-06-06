// Aarón Josué Meza Torres B74787

#include "utils.h"
#include <sstream>
#include <vector>
#include <stdexcept>
#include <bitset>

uint32_t ipToUint(const std::string &ipString) {
    std::istringstream stream(ipString);
    std::string segment;
    uint32_t result = 0;

    for (int i = 0; i < 4; ++i) {
        if (!std::getline(stream, segment, '.')) {
            throw std::runtime_error("Invalid IP address format: " + ipString);
        }

        int octect = std::stoi(segment);
        if (octect < 0 || octect > 255) {
            throw std::runtime_error("IP address octet out of range: " + segment);
        }
        result = (result << 8) | static_cast<uint32_t>(octect);
    }
    return result;
}

std::string uintToIP(uint32_t ipUint) {
    std::ostringstream stream;
    for (int i = 3; i >= 0; --i) {
        uint32_t octect = (ipUint >> (i * 8)) & 0xFF;
        stream << static_cast<int>(octect);
        if (i > 0) {
            stream << '.';
        }
    }
    return stream.str();
}

uint32_t prefixToMask(uint8_t prefix) {
    if (prefix > 32) {
        throw std::runtime_error("Invalid prefix length: " + std::to_string(prefix));
    }
    if (prefix == 0) {
        return 0x00000000;
    }
    return (0xFFFFFFFF << (32 - prefix));
}

std::string maskToString(uint32_t mask) {
    std::ostringstream stream;
    for (int i = 3; i >= 0; --i) {
        uint32_t octect = (mask >> (i * 8)) & 0xFF;
        stream << static_cast<int>(octect);
        if (i > 0) {
            stream << '.';
        }
    }
    return stream.str();
}