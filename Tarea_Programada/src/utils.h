#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <cstdint>

uint32_t ipToUint(const std::string& ipStr);
std::string uintToIP(uint32_t ip);
uint32_t prefixToMask(uint8_t prefix);
std::string maskToString(uint32_t mask);
void parseCIDR(const std::string& cidr, uint32_t& ip, uint8_t& prefix);

#endif // UTILS_H
