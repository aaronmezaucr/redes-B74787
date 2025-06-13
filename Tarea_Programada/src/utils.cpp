#include "utils.h"
#include <sstream>
#include <stdexcept>
#include <vector>
#include <iomanip>

// Convierte una IP en string "x.y.z.w" a uint32_t
uint32_t ipToUint(const std::string& ipStr) {
    std::istringstream iss(ipStr);
    std::string token;
    uint32_t ip = 0;
    for (int i = 0; i < 4; i++) {
        if (!std::getline(iss, token, '.')) {
            throw std::invalid_argument("Invalid IP format: " + ipStr);
        }
        int octet = std::stoi(token);
        if (octet < 0 || octet > 255) {
            throw std::out_of_range("IP octet out of range: " + token);
        }
        ip = (ip << 8) | static_cast<uint32_t>(octet);
    }
    return ip;
}

// Convierte uint32_t IP a string "x.y.z.w"
std::string uintToIP(uint32_t ip) {
    std::ostringstream oss;
    oss << ((ip >> 24) & 0xFF) << "."
        << ((ip >> 16) & 0xFF) << "."
        << ((ip >> 8) & 0xFF) << "."
        << (ip & 0xFF);
    return oss.str();
}

// Convierte un prefijo (ej. 24) a máscara en uint32_t (ej. 0xFFFFFF00)
uint32_t prefixToMask(uint8_t prefix) {
    if (prefix > 32) {
        throw std::out_of_range("Prefix must be between 0 and 32");
    }
    if (prefix == 0) return 0;
    return 0xFFFFFFFF << (32 - prefix);
}

// Convierte una máscara uint32_t a string "x.y.z.w"
std::string maskToString(uint32_t mask) {
    return uintToIP(mask);
}

// Parsea un CIDR como "192.168.1.0/24" en IP y prefijo
void parseCIDR(const std::string& cidr, uint32_t& ip, uint8_t& prefix) {
    size_t slashPos = cidr.find('/');
    if (slashPos == std::string::npos) {
        throw std::invalid_argument("CIDR format missing '/'");
    }

    std::string ipStr = cidr.substr(0, slashPos);
    std::string prefixStr = cidr.substr(slashPos + 1);

    ip = ipToUint(ipStr);

    int prefixInt = std::stoi(prefixStr);
    if (prefixInt < 0 || prefixInt > 32) {
        throw std::out_of_range("Prefix out of valid range (0-32)");
    }
    prefix = static_cast<uint8_t>(prefixInt);
}
