#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <algorithm>

#include "utils.h"
#include "subnetting.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <base_network_file> <requests_file> <mode>" << std::endl;
        return 1;
    }

    std::string baseNetworkFile = argv[1];
    std::string requestsFile = argv[2];
    std::string mode = argv[3];

    // Leer la dirección de red base con prefijo
    std::ifstream baseNetworkStream(baseNetworkFile);
    if (!baseNetworkStream.is_open()) {
        std::cerr << "Error opening base network file: " << baseNetworkFile << std::endl;
        return 1;
    }

    std::string baseNetwork;
    std::getline(baseNetworkStream, baseNetwork);
    baseNetworkStream.close();

    uint32_t networkAddress = 0;
    uint8_t basePrefix = 0;

    try {
        parseCIDR(baseNetwork, networkAddress, basePrefix);
    } catch (const std::exception& e) {
        std::cerr << "Error parsing base network: " << e.what() << std::endl;
        return 1;
    }

    // Leer solicitudes
    std::ifstream requestsStream(requestsFile);
    if (!requestsStream.is_open()) {
        std::cerr << "Error opening requests file: " << requestsFile << std::endl;
        return 1;
    }

    std::vector<request> requestsList;
    std::string name;
    uint32_t hostsNumber;
    uint32_t entry_ID = 0;
    while (requestsStream >> name >> hostsNumber) {
        request req;
        req.name = name;
        req.hostsNumber = hostsNumber;
        req.entry_ID = entry_ID++;
        requestsList.push_back(req);
    }
    requestsStream.close();

    try {
        // Procesar solicitudes (ahora con el prefijo correcto desde el archivo)
        processRequests(networkAddress, basePrefix, requestsList, mode);

        // Ordenar por ID original para mantener el orden de entrada
        std::sort(requestsList.begin(), requestsList.end(),
                  [](const auto& a, const auto& b) {
                      return a.entry_ID < b.entry_ID;
                  });

        // Mostrar resultados
        std::cout << "Name | Hosts | Network Address | Mask | Broadcast" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
        for (const auto& s : requestsList) {
            std::string networkAddressStr = uintToIP(s.assignedIP);
            uint32_t mask = prefixToMask(s.prefix);
            std::string maskStr = maskToString(mask);
            uint32_t broadcast = s.assignedIP + s.subnetSize - 1;
            std::string broadcastStr = uintToIP(broadcast);
            
            std::cout << s.name << " | "
                      << s.hostsNumber << " | "
                      << networkAddressStr << "/" << static_cast<int>(s.prefix) << " | "
                      << maskStr << " | "
                      << broadcastStr << std::endl;
        }

    } catch (const std::runtime_error& e) {
        std::cerr << "Error processing requests: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
