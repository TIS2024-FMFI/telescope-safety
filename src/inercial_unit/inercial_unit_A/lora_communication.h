
#ifndef LORA_COMMUNICATION_H
#define LORA_COMMUNICATION_H

#include <SPI.h>
#include <LoRa.h>
#include "transform_to_azimuth_elevation.h"
#include "reset_sensor.h"

// Konfigurácia LoRa
#define LORA_CS_PIN 10
#define LORA_RESET_PIN 9
#define LORA_IRQ_PIN 2
#define LORA_FREQUENCY 868E6

#define localAddress 0xBB

void initializeLoRa();

// Sends AzimuthElevation structure to control unit
// @param azimutElevation pointer to AzimuthElevation structure
// @return 0 if success, -1 if error
int sendToControlUnit(AzimuthElevation* azimuthElevation);

// Receiving message from control unit to restart
// @return 0 if success, -1 if error
int readFromControlUnit();

#endif // LORA_COMMUNICATION_H
