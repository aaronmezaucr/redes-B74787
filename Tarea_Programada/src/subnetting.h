// Aarón Josué Meza Torres B74787

#pragma once

#include <string>
#include <vector>
#include <cstdint>

struct request {
    std::string name;
    uint32_t hostsNumber;  // Cantidad de hosts solicitados
    uint32_t subnetSize;  // Cantidad de hosts que puede soportar la subred
    uint8_t prefix;  // Notación CIDR
    uint32_t assignedIP;  // Dirección de la red asignada
    uint32_t entry_ID;  // Orden de entrada en la lista de solicitudes
};

void processRequests(
    uint32_t networkAddress,
    uint8_t basePrefix,
    std::vector<request>& requestsList,
    const std::string& mode
);  // Modo de asignación de subredes