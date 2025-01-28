
#ifndef LORA_COMMUNICATION_H
#define LORA_COMMUNICATION_H

#include <SPI.h>
#include <LoRa.h>
#include "common_structs.h"

// Konfigurácia LoRa
#define LORA_CS_PIN 13
#define LORA_RESET_PIN 9
#define LORA_IRQ_PIN 2
#define LORA_FREQUENCY 868E6

#define localAddress 0xAA

void initializeLoRa();

// Reads data from inertial unit and stores them in AzimuthElevation structure
// @return pointer to AzimuthElevation structure
AzimuthElevation* readFromInertialUnit();

int restartInertialUnit(double azimuth = -1);

#endif // LORA_COMMUNICATION_H
