#ifndef IMU_MODULE_H
#define IMU_MODULE_H

#define IMU_MODULE_ADDRESS   (0xdd >> 1)

#define IMU_OPCODE_CONFIGURE (0x1)
#define IMU_OPCODE_READ      (0x2)

typedef struct {
  uint8_t hz;
} imu_configuration_t;

typedef struct {
  uint8_t opcode;
} imu_command_t;

typedef struct {
  float heading;
  float yaw;
  float pitch;
  float roll;
} imu_orientation_t;

#endif
