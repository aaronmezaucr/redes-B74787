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
    for (auto &s : requestsList) {
        uint32_t mask = prefixToMask(s.prefix);
        uint32_t subnetAddress = cursor & mask;

        if (subnetAddress < cursor) {
            uint32_t nextJump = (1u << (32 - s.prefix));
            subnetAddress = ((cursor >> (32 - s.prefix)) + 1) << (32 - s.prefix);
        }
        
        if (subnetAddress + s.subnetSize - 1 > (networkAddress + blockSize - 1)) {
            throw std::runtime_error("Insufficient space for subnet: " + std::to_string(s.hostsNumber));
        }
        s.assignedIP = subnetAddress;
        cursor = subnetAddress + s.subnetSize;
    }
}

static void descendingAssignment(
    uint32_t networkAddress,
    uint8_t basePrefix,
    uint32_t blockSize,
    std::vector<request>& requestsList
) {
    uint32_t blockEnd = networkAddress + blockSize - 1;
    uint32_t cursorStop = blockEnd;

    for (auto &s : requestsList) {
        uint32_t ipNetCandidate = cursorStop - (s.subnetSize - 1);
        uint32_t mask = prefixToMask(s.prefix);
        uint32_t subnetAddress = ipNetCandidate & mask;
        uint32_t broadcast =  subnetAddress + s.subnetSize - 1;

        if (broadcast > blockEnd) {
            uint32_t jump = (1u << (32 - s.prefix));
            subnetAddress = ((ipNetCandidate >> (32 - s.prefix)) - 1) << (32 - s.prefix);
            broadcast = subnetAddress + s.subnetSize - 1;
        }
        if ( subnetAddress < networkAddress) {
            throw std::runtime_error("Insufficient space for subnet: " + std::to_string(s.hostsNumber));
        }
        s.assignedIP = subnetAddress;
        cursorStop = subnetAddress - 1;
    }
}

void processRequests(
    uint32_t networkAddress,
    uint8_t basePrefix,
    std::vector<request>& requestsList,
    const std::string& mode
) {
    calculatePrefix(requestsList);

    std::sort(requestsList.begin(), requestsList.end(),
              [](auto& a, auto& b) {
                  return a.subnetSize < b.subnetSize;
              });

    uint32_t blockSize = (1U << (32 - basePrefix));

    if (mode == "ascending") {
        ascendingAssignment(networkAddress, basePrefix, blockSize, requestsList);
    } else if (mode == "descending") {
        descendingAssignment(networkAddress, basePrefix, blockSize, requestsList);
    } else {
        throw std::runtime_error("Invalid mode: " + mode);
    }

}