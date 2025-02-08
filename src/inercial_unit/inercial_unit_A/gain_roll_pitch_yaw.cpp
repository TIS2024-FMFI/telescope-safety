
#include "gain_roll_pitch_yaw.h"

double yawOffset=0.0;

uint8_t AD0_VAL = 0;

#ifdef USE_SPI
ICM_20948_SPI myICM;
#else
ICM_20948_I2C myICM;
#endif

void initializeSensor() {
#ifdef USE_SPI
    SPI_PORT.begin();
#else
    WIRE_PORT.begin();
    WIRE_PORT.setClock(400000);
#endif

    bool initialized = false;
    while (!initialized) {
#ifdef USE_SPI
        myICM.begin(CS_PIN, SPI_PORT);
#else
        ICM_20948_Status_e status = myICM.begin(WIRE_PORT, AD0_VAL);
        if(status!=ICM_20948_Stat_Ok){
          AD0_VAL=1;
          myICM.begin(WIRE_PORT, AD0_VAL);
        }
#endif
        if (myICM.status != ICM_20948_Stat_Ok) {
            delay(500);
        } else {
            initialized = true;
        }
    }

    bool success = true;
    success &= (myICM.initializeDMP() == ICM_20948_Stat_Ok);
    success &= (myICM.enableDMPSensor(INV_ICM20948_SENSOR_GAME_ROTATION_VECTOR) == ICM_20948_Stat_Ok);
    success &= (myICM.setDMPODRrate(DMP_ODR_Reg_Quat6, 0) == ICM_20948_Stat_Ok);
    success &= (myICM.enableFIFO() == ICM_20948_Stat_Ok);
    success &= (myICM.enableDMP() == ICM_20948_Stat_Ok);
    success &= (myICM.resetDMP() == ICM_20948_Stat_Ok);
    success &= (myICM.resetFIFO() == ICM_20948_Stat_Ok);

    if (!success) {
        SERIAL_PORT.println(F("Enable DMP failed!"));
        while (1);
    }
}

RollPitchYaw* readFromSensor() {
    static RollPitchYaw roll_pitch_yaw;
    icm_20948_DMP_data_t data;

    myICM.readDMPdataFromFIFO(&data);

    if ((myICM.status == ICM_20948_Stat_Ok) || (myICM.status == ICM_20948_Stat_FIFOMoreDataAvail)) {
        if ((data.header & DMP_header_bitmap_Quat6) > 0) {
            double q1 = ((double)data.Quat6.Data.Q1) / 1073741824.0; // Convert to double. Divide by 2^30
            double q2 = ((double)data.Quat6.Data.Q2) / 1073741824.0; // Convert to double. Divide by 2^30
            double q3 = ((double)data.Quat6.Data.Q3) / 1073741824.0; // Convert to double. Divide by 2^30

            double q0 = sqrt(1.0 - ((q1 * q1) + (q2 * q2) + (q3 * q3)));

            double qw = q0;
            double qx = q2;
            double qy = q1;
            double qz = -q3;

            // roll (x-axis rotation)
            double t0 = +2.0 * (qw * qx + qy * qz);
            double t1 = +1.0 - 2.0 * (qx * qx + qy * qy);
            roll_pitch_yaw.roll = atan2(t0, t1) * 180.0 / PI;

            // pitch (y-axis rotation)
            double t2 = +2.0 * (qw * qy - qx * qz);
            t2 = t2 > 1.0 ? 1.0 : t2;
            t2 = t2 < -1.0 ? -1.0 : t2;
            roll_pitch_yaw.pitch = asin(t2) * 180.0 / PI;

            // yaw (z-axis rotation)
            double t3 = +2.0 * (qw * qz + qx * qy);
            double t4 = +1.0 - 2.0 * (qy * qy + qz * qz);
            roll_pitch_yaw.yaw = atan2(t3, t4) * 180.0 / PI;

            roll_pitch_yaw.yaw += yawOffset;
            if (roll_pitch_yaw.yaw > 180.0) roll_pitch_yaw.yaw -= 360.0;
            if (roll_pitch_yaw.yaw < -180.0) roll_pitch_yaw.yaw += 360.0;
        }
    }

    return &roll_pitch_yaw;
}
