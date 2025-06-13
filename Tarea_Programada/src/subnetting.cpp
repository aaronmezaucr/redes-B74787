// Aarón Josué Meza Torres B74787

#include "subnetting.h"
#include "utils.h"
#include <algorithm>
#include <stdexcept>
#include <cstdint>
#include <numeric>

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
    // Crear vector de índices para mantener referencia a la posición
    std::vector<size_t> indices(requestsList.size());
    std::iota(indices.begin(), indices.end(), 0);

    // Ordenar: primero por tamaño descendente, luego por nombre ascendente
    std::sort(indices.begin(), indices.end(), [&](size_t i, size_t j) {
        if (requestsList[i].subnetSize != requestsList[j].subnetSize) {
            return requestsList[i].subnetSize > requestsList[j].subnetSize;
        }
        return requestsList[i].name < requestsList[j].name;  // Alfabético
    });

    uint32_t cursor = networkAddress;
    uint32_t blockEnd = networkAddress + blockSize;

    for (auto i : indices) {
        auto& s = requestsList[i];

        // Alinear la IP a un múltiplo del tamaño de subred
        uint32_t alignedCursor = (cursor + s.subnetSize - 1) & ~(s.subnetSize - 1);
        if (alignedCursor + s.subnetSize > blockEnd) {
            throw std::runtime_error("Insufficient space for subnet: " + std::to_string(s.hostsNumber));
        }

        s.assignedIP = alignedCursor;
        cursor = alignedCursor + s.subnetSize;
    }
}



static void descendingAssignment(
    uint32_t networkAddress,
    uint8_t basePrefix,
    uint32_t blockSize,
    std::vector<request>& requestsList
) {
    // Crear vector de índices y ordenar
    std::vector<size_t> indices(requestsList.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(), [&](size_t i, size_t j) {
        // Orden principal: tamaño de subred descendente
        if (requestsList[i].subnetSize != requestsList[j].subnetSize) {
            return requestsList[i].subnetSize > requestsList[j].subnetSize;
        }
        // Orden secundario: índice descendente (mismo tamaño)
        return i > j;  // Solicitudes posteriores primero
    });

    uint32_t blockEnd = networkAddress + blockSize;
    uint32_t high_cursor = blockEnd;

    for (auto i : indices) {
        auto& s = requestsList[i];
        high_cursor -= s.subnetSize;
        s.assignedIP = high_cursor;

        // Validación de alineación
        uint32_t subnetMask = ~(s.subnetSize - 1);
        if ((s.assignedIP & subnetMask) != s.assignedIP) {
            throw std::runtime_error("Alineación inválida para tamaño: " + std::to_string(s.subnetSize));
        }

        // Validación de rango
        if (s.assignedIP < networkAddress) {
            throw std::runtime_error("Espacio insuficiente: " + std::to_string(s.hostsNumber));
        }
    }
}

void processRequests(
    uint32_t networkAddress,
    uint8_t basePrefix,
    std::vector<request>& requestsList,
    const std::string& mode
) {
    calculatePrefix(requestsList);
    uint32_t blockSize = (1U << (32 - basePrefix));

    if (mode == "ascending") {
        ascendingAssignment(networkAddress, basePrefix, blockSize, requestsList);
    } else if (mode == "descending") {
        descendingAssignment(networkAddress, basePrefix, blockSize, requestsList);
    } else {
        throw std::runtime_error("Invalid mode: " + mode);
    }
}
