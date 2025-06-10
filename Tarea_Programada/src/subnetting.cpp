// Aarón Josué Meza Torres B74787

#include "subnetting.h"
#include "utils.h"
#include <algorithm>
#include <stdexcept>
#include <cstdint>

static void calculatePrefix(std::vector<request>& requestsList) {
    for (auto &s : requestsList) {
        uint32_t hostBits = 1;
        while ((1U << hostBits) - 2 < s.hostsNumber) {
            hostBits++;
            if (hostBits > 30) {
                throw std::runtime_error("Too many hosts requested: " + std::to_string(s.hostsNumber));
            }
        }
        s.subnetSize = (1U << hostBits);
        s.prefix = 32 - hostBits;
    }
}

static void ascendingAssignment(
    uint32_t networkAddress,
    uint8_t basePrefix,
    uint32_t blockSize,
    std::vector<request>& requestsList
) {
    uint32_t cursor = networkAddress;
    uint32_t blockEnd = networkAddress + blockSize - 1;

    for (auto &s : requestsList) {
        if (cursor + s.subnetSize - 1 > blockEnd) {
            throw std::runtime_error("Insufficient space for subnet: " + std::to_string(s.hostsNumber));
        }
        s.assignedIP = cursor;
        cursor += s.subnetSize;
    }
}



void processRequests(
    uint32_t networkAddress,
    uint8_t basePrefix,
    std::vector<request>& requestsList,
    const std::string& mode
) {
    calculatePrefix(requestsList);

    if (mode == "ascending") {
        // Ordenar de menor a mayor tamaño para que las subredes pequeñas estén primero
        std::sort(requestsList.begin(), requestsList.end(),
                  [](auto& a, auto& b) {
                      return a.subnetSize < b.subnetSize;
                  });
    } else if (mode == "descending") {
        // Ordenar de mayor a menor tamaño para que las subredes grandes estén primero
        std::sort(requestsList.begin(), requestsList.end(),
                  [](auto& a, auto& b) {
                      return a.subnetSize > b.subnetSize;
                  });
    } else {
        throw std::runtime_error("Invalid mode: " + mode);
    }

    uint32_t blockSize = (1U << (32 - basePrefix));
    ascendingAssignment(networkAddress, basePrefix, blockSize, requestsList);
}
