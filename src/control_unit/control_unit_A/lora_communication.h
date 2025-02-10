#ifndef LORA_COMMUNICATION_H
#define LORA_COMMUNICATION_H

#include <SPI.h>
#include <LoRa.h>
#include "common_structs.h"
#include "danger_evaluation.h"
#include "servers.h"
#include "logs.h"
#include "buttons.h"

// Konfigurácia LoRa
#define LORA_CS_PIN 13
#define LORA_RESET_PIN 9
#define LORA_IRQ_PIN 2
#define LORA_FREQUENCY 868E6

#define localAddress 0xAA

// Sets up the lora pins and frequencies
void initializeLoRa();

// Reads data from inertial unit and stores them in AzimuthElevation structure
// @return pointer to AzimuthElevation structure
AzimuthElevation* readFromInertialUnit();

// Restarts inertial unit
// @return 0 if success, -1 if error
// @param azimuth value indicates calibration of azimuth on inertial unit
// if its -1 it stays the same and sensor just resets
int restartInertialUnit(double azimuth = -1, int calibrationMatrix[3][3] = nullptr);

// Display given Azimuth and Elevation on the display
// @param ae AzimuthElevation struct that should be displayed
void displayAE(AzimuthElevation* ae);

// Reads from senzor, evaluates danger, logs, displays on screen and sends to connected clients
void doOperations();

#endif // LORA_COMMUNICATION_H
