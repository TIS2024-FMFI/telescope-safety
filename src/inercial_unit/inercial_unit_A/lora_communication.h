
#ifndef LORA_COMMUNICATION_H
#define LORA_COMMUNICATION_H

#include <SPI.h>
#include <LoRa.h>
#include "transform_to_azimuth_elevation.h"

// Konfigurácia LoRa
#define LORA_CS_PIN 10
#define LORA_RESET_PIN 9
#define LORA_IRQ_PIN 2
#define LORA_FREQUENCY 868E6

#define localAddress 0xBB

void initializeLoRa();

int sendToControlUnit(AzimuthElevation* azimuthElevation);

int readFromControlUnit();

#endif // LORA_COMMUNICATION_H
