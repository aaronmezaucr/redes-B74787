// Aarón Josué Meza Torres B74787

#pragma once

#include <string>
#include <cstdint>

// Convierte una dirección IP en formato de string a un entero de 32 bits
uint32_t ipToUint (const std::string &ipString);
// Convierte un entero de 32 bits a una dirección IP en formato de string
std::string uintToIP (uint32_t ipUint);
// Convierte un prefijo de red (CIDR) a una máscara de subred en formato de entero de 32 bits
uint32_t prefixToMask (uint8_t prefix);
// Convierte una máscara de subred en formato de entero de 32 bits a un string
std::string maskToString (uint32_t mask);

