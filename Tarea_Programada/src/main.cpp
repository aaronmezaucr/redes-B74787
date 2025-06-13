#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <iomanip>

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

       // Encabezado formateado con setw y left/right
        std::cout 
        << std::left  << std::setw(8) << "Name" 
        << " | " << std::right << std::setw(8) << "Requested" 
        << " | " << std::setw(8) << "Assigned" 
        << " | " << std::left  << std::setw(18) << "Network" 
        << " | " << std::setw(15) << "Mask" 
        << " | " << std::setw(21) << "Host range" 
        << " | " << "Broadcast"
        << "\n";

        // Línea de separación (ajusta el número según la suma de anchuras + separadores)
        std::cout << std::string(8 + 3 + 4 + 3 + 4 + 3 + 18 + 3 + 15 + 3 + 21 + 3 + 9, '-') << "\n";

        for (const auto& s : requestsList) {
            // IP de red y máscara
            std::string networkAddressStr = uintToIP(s.assignedIP);
            std::string cidrStr           = networkAddressStr + "/" + std::to_string(s.prefix);
            uint32_t mask                 = prefixToMask(s.prefix);
            std::string maskStr           = maskToString(mask);

            // Broadcast
            uint32_t broadcast = s.assignedIP + s.subnetSize - 1;
            std::string broadcastStr = uintToIP(broadcast);

            // Hosts
            uint32_t requestedHosts = s.hostsNumber;
            uint32_t assignedHosts  = s.subnetSize - 2;

            // Rango de hosts utilizable
            uint32_t firstHost = s.assignedIP + 1;
            uint32_t lastHost  = broadcast - 1;
            std::string hostRangeStr = uintToIP(firstHost) + " - " + uintToIP(lastHost);

            // Imprime todas las columnas
            std::cout
                << std::left << std::setw(8)  << s.name
                << " | " << std::right << std::setw(8)  << requestedHosts
                << " | " << std::setw(8)  << assignedHosts
                << " | " << std::setw(18) << cidrStr
                << " | " << std::setw(15) << maskStr
                << " | " << std::setw(21) << hostRangeStr
                << " | " << broadcastStr
                << "\n";
        }

    } catch (const std::runtime_error& e) {
        std::cerr << "Error processing requests: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
