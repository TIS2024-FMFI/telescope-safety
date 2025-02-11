
#include "gain_roll_pitch_yaw.h"

SensorType detectedSensor=SENSOR_UNKNOWN;

//definitions needed for icm sensors
uint8_t AD0_VAL = 0;                  //
                                      //
#ifdef USE_SPI                        //
ICM_20948_SPI myICM;                  //
#else                                 //
ICM_20948_I2C myICM;                  //
#endif                                //
//

//definitions needed for ism sensor
static unsigned long last_us = 0, now_us = 0;  //

SparkFun_ISM330DHCX ISM330;                    //

sfe_ism_data_t acc;                            //
sfe_ism_data_t gyr;                            //

#define CAL_GYRO                               //
float gxyz_offsets[3] = {0};                   //

float RadiansPerSecond = PI / 180000.0; //conversion factor for Mahony filter
float Axyz[3] = {0}, Gxyz[3] = {0}; //arrays for Mahony filter

float Kp = 50.0, Ki = 0.0; //PID constants for Mahony filter
float q[4] = {1.0, 0.0, 0.0, 0.0}; //initialize quaternion, global access. X points at yaw=0
//

SensorType detectSensor() {
    Wire.begin();

    Wire.beginTransmission(0x68);
    if (Wire.endTransmission() == 0) return SENSOR_ICM20948;

    Wire.beginTransmission(0x69);
    if (Wire.endTransmission() == 0) return SENSOR_ICM20948;

    Wire.beginTransmission(0x6B);
    if (Wire.endTransmission() == 0) return SENSOR_ISM330DHCX;

    return SENSOR_UNKNOWN; // No sensor detected
}

void initializeSensor() {
  detectedSensor = detectSensor();
  if (detectedSensor == SENSOR_ICM20948) {
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
    else if (detectedSensor == SENSOR_ISM330DHCX) {
    // Reset quaternion values to initial state
    q[0] = 1.0;
    q[1] = 0.0;
    q[2] = 0.0;
    q[3] = 0.0;

#if defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2_TFT)
    pinMode(TFT_I2C_POWER, OUTPUT);
    digitalWrite(TFT_I2C_POWER, HIGH);
#endif

    Wire.begin();

    Serial.begin(115200);
    while (!Serial);  // Wait for serial monitor to connect (or replace with delay())

    if (!ISM330.begin(0x6B)) {  // 0x6B is the default address
        Serial.println("ISM330 not detected.");
        while (1) delay(1);
    }

    ISM330.setDeviceConfig();
    ISM330.setBlockDataUpdate();

    // Set accelerometer data rate and full scale
    ISM330.setAccelDataRate(ISM_XL_ODR_104Hz);
    ISM330.setAccelFullScale(ISM_2g);

    // Set gyroscope data rate and full scale
    ISM330.setGyroDataRate(ISM_GY_ODR_104Hz);
    ISM330.setGyroFullScale(ISM_500dps);

    // Enable filters
    ISM330.setGyroFilterLP1();
    ISM330.setGyroLP1Bandwidth(ISM_MEDIUM);

#ifdef CAL_GYRO
#define GYRO_SAMPLES 500
    Serial.println("Keep gyro still for offset calculation");
    delay(2000);

    // Reset gyro offsets
    gxyz_offsets[0] = 0;
    gxyz_offsets[1] = 0;
    gxyz_offsets[2] = 0;

    int loopcount = 0;
    while (loopcount < GYRO_SAMPLES) {  // Accumulate sums
        if (ISM330.checkStatus()) {
            ISM330.getGyro(&gyr);
            gxyz_offsets[0] += gyr.xData;
            gxyz_offsets[1] += gyr.yData;
            gxyz_offsets[2] += gyr.zData;
            loopcount++;
        }
    }

    // Calculate and store gyro offsets
    for (int i = 0; i < 3; i++) gxyz_offsets[i] /= (float)loopcount;
#endif

    // Print offsets
    Serial.print("Gyro offsets: ");
    for (int i = 0; i < 3; i++) {
        Serial.print(gxyz_offsets[i], 1);
        Serial.print(" ");
    }
    Serial.println();
}
}

RollPitchYaw* readFromSensor() {
  static RollPitchYaw roll_pitch_yaw;
  if (detectedSensor == SENSOR_ICM20948) {
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
        }
    }

    }
    else if (detectedSensor == SENSOR_ISM330DHCX) {
      if ( ISM330.checkStatus() ) {
        ISM330.getAccel(&acc);
        ISM330.getGyro(&gyr);
        now_us = micros();

        float amag = sqrt(acc.xData * acc.xData + acc.yData * acc.yData + acc.zData * acc.zData);

          Axyz[0] = acc.xData / amag;
          Axyz[1] = acc.yData / amag;
          Axyz[2] = acc.zData / amag;

          Gxyz[0] = (gyr.xData - gxyz_offsets[0]) * RadiansPerSecond;
          Gxyz[1] = (gyr.yData - gxyz_offsets[1]) * RadiansPerSecond;
          Gxyz[2] = (gyr.zData - gxyz_offsets[2]) * RadiansPerSecond;

          float deltat = (now_us - last_us) * 1.0e-6; //seconds since last update
          last_us = now_us;

          Mahony_update(Axyz[0], Axyz[1], Axyz[2], Gxyz[0], Gxyz[1], Gxyz[2], deltat);

          float roll  = atan2((q[0] * q[1] + q[2] * q[3]), 0.5 - (q[1] * q[1] + q[2] * q[2]));
          float pitch = asin(2.0 * (q[0] * q[2] - q[1] * q[3]));
          //conventional yaw increases clockwise from North. Not that the MPU-6050 knows where North is.
          float yaw   = -atan2((q[1] * q[2] + q[0] * q[3]), 0.5 - (q[2] * q[2] + q[3] * q[3]));
          // to degrees
          yaw   *= 180.0 / PI;
          pitch *= 180.0 / PI;
          roll *= 180.0 / PI;

          roll_pitch_yaw.yaw=yaw;
          roll_pitch_yaw.pitch=pitch;
          roll_pitch_yaw.roll=roll;

          }
  
}
return &roll_pitch_yaw;
}

void Mahony_update(float ax, float ay, float az, float gx, float gy, float gz, float deltat) {
  float recipNorm;
  float vx, vy, vz;
  float ex, ey, ez;  //error terms
  float qa, qb, qc;
  static float ix = 0.0, iy = 0.0, iz = 0.0;  //integral feedback terms

  // Estimated direction of gravity in the body frame (factor of two divided out)
  vx = q[1] * q[3] - q[0] * q[2];  //to normalize these terms, multiply each by 2.0
  vy = q[0] * q[1] + q[2] * q[3];
  vz = q[0] * q[0] - 0.5f + q[3] * q[3];

  // Error is cross product between estimated and measured direction of gravity in body frame
  // (half the actual magnitude)
  ex = (ay * vz - az * vy);
  ey = (az * vx - ax * vz);
  ez = (ax * vy - ay * vx);

  // Compute and apply to gyro term the integral feedback, if enabled
  if (Ki > 0.0f) {
    ix += Ki * ex * deltat;  // integral error scaled by Ki
    iy += Ki * ey * deltat;
    iz += Ki * ez * deltat;
    gx += ix;  // apply integral feedback
    gy += iy;
    gz += iz;
  }

  // Apply proportional feedback to gyro term
  gx += Kp * ex;
  gy += Kp * ey;
  gz += Kp * ez;

  // Integrate rate of change of quaternion, q cross gyro term
  deltat = 0.5 * deltat;
  gx *= deltat;   // pre-multiply common factors
  gy *= deltat;
  gz *= deltat;
  qa = q[0];
  qb = q[1];
  qc = q[2];
  q[0] += (-qb * gx - qc * gy - q[3] * gz);
  q[1] += (qa * gx + qc * gz - q[3] * gy);
  q[2] += (qa * gy - qb * gz + q[3] * gx);
  q[3] += (qa * gz + qb * gy - qc * gx);

  // renormalise quaternion
  recipNorm = 1.0 / sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
  q[0] = q[0] * recipNorm;
  q[1] = q[1] * recipNorm;
  q[2] = q[2] * recipNorm;
  q[3] = q[3] * recipNorm;
}
