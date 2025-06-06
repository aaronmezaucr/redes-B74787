// Aarón Josué Meza Torres B74787

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
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file> <subnet_mask>" << std::endl;
        return 1;       
    }

    std::string baseNetwork = argv[1];
    std::string requests = argv[2];
    std::string mode = argv[3];

    std::ifstream baseNetworkFile(baseNetwork);
    if (!baseNetworkFile.is_open()) {
        std::cerr << "Error opening base network file: " << baseNetwork << std::endl;
        return 1;
    }

    std::string baseNetworkLine;
    std::getline(baseNetworkFile, baseNetworkLine);
    baseNetworkFile.close();

    size_t slash = baseNetworkLine.find('/');
    if (slash == std::string::npos) {
        std::cerr << "Invalid base network format. Expected format: <network>/<subnet_mask>" << std::endl;
        return 1;
    }

    std::string network = baseNetworkLine.substr(0, slash);
    std::string subnetMask = baseNetworkLine.substr(slash + 1);
    
    uint32_t networkAddress = 0;
    uint8_t basePrefix = 0;

    try {
        networkAddress = ipToUint(network);
        basePrefix = static_cast<uint8_t>(std::stoi(subnetMask));
        if (basePrefix > 32) {
            throw std::invalid_argument("Invalid subnet mask: " + subnetMask);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing base network: " << e.what() << std::endl;
        return 1;
    }

    std::ifstream fileRequests(requests);
    if (!fileRequests.is_open()) {
        std::cerr << "Error opening requests file: " << requests << std::endl;
        return 1;
    }

    std::vector<request> requestsList;
    std::string name;
    uint32_t hostsNumber;
    uint32_t entry_ID = 0;
    while (fileRequests >> name >> hostsNumber) {
        request request;
        request.name = name;
        request.hostsNumber = hostsNumber;
        request.entry_ID = entry_ID;
        requestsList.push_back(request);
    }
    fileRequests.close();

    try {
        processRequests(networkAddress, basePrefix, requestsList, mode);

        std::sort(requestsList.begin(), requestsList.end(),
                  [](auto a, auto b) {
                      return a.entry_ID < b.entry_ID;
                  });

        std::cout << "Name | Hosts | Network Address | Mask" << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        for (auto &s : requestsList) {
            std::string networkAddressString = uintToIP(s.assignedIP);
            uint32_t mask = prefixToMask(s.prefix);
            std::string maskString = maskToString(mask);
            std::cout << s.name << " | "
                      << s.hostsNumber << " | "
                      << networkAddressString << " | "
                      << maskString << std::endl;
        }

    } catch (const std::runtime_error& e) {
        std::cerr << "Error processing requests: " << e.what() << std::endl;
        return 1;
    }

}