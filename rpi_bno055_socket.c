/*
 * @Author: Brogan Miner
 * @Date:   Thursday November 14th 2019
 * @Last Modified By:  Brogan Miner
 * @Last Modified Time:  Thursday November 14th 2019
 * @Copyright:  (c) Your Company 2019
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "lib/BNO055_driver/bno055.h"

#define	I2C_BUFFER_LEN 8
#define I2C0 5
#define	BNO055_I2C_BUS_WRITE_ARRAY_INDEX	((u8)1)

s8 BNO055_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt) {
  s32 BNO055_iERROR = BNO055_INIT_VALUE;
	u8 array[I2C_BUFFER_LEN];
	u8 stringpos = BNO055_INIT_VALUE;

	array[BNO055_INIT_VALUE] = reg_addr;
	for (stringpos = BNO055_INIT_VALUE; stringpos < cnt; stringpos++) {
		array[stringpos + BNO055_I2C_BUS_WRITE_ARRAY_INDEX] =
			*(reg_data + stringpos);
	}

  int file_i2c;

	char *filename = (char*)"/dev/i2c-1";
	if ((file_i2c = open(filename, O_RDWR)) < 0) {
		printf("Failed to open the i2c bus");
	}
	
	if (ioctl(file_i2c, I2C_SLAVE, dev_addr) < 0) {
		printf("Failed to acquire bus access and/or talk to slave.\n");
	}

  if (write(file_i2c, array, I2C_BUFFER_LEN) != I2C_BUFFER_LEN) {
		printf("Failed to write to the i2c bus.\n");
	}

  close(file_i2c);

  return (s8)BNO055_iERROR;
}

s8 BNO055_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt) {
  s32 BNO055_iERROR = BNO055_INIT_VALUE;
	u8 array[I2C_BUFFER_LEN] = {BNO055_INIT_VALUE};
	u8 stringpos = BNO055_INIT_VALUE;

	array[BNO055_INIT_VALUE] = reg_addr;

  int file_i2c;

	char *filename = (char*)"/dev/i2c-1";
	if ((file_i2c = open(filename, O_RDWR)) < 0) {
		printf("Failed to open the i2c bus");
	}
	
	if (ioctl(file_i2c, I2C_SLAVE, dev_addr) < 0) {
		printf("Failed to acquire bus access and/or talk to slave.\n");
	}

  if (read(file_i2c, array, I2C_BUFFER_LEN) != I2C_BUFFER_LEN) {
		printf("Failed to read from the i2c bus.\n");
	}

  close(file_i2c);

  for (stringpos = BNO055_INIT_VALUE; stringpos < cnt; stringpos++)
		*(reg_data + stringpos) = array[stringpos];
	return (s8)BNO055_iERROR;
}

void BNO055_delay_msek(u32 msek) {
  sleep((double)msek / 1000.0);
}

s8 I2C_routine(struct bno055_t* bno055) {
	bno055->bus_write = BNO055_I2C_bus_write;
	bno055->bus_read = BNO055_I2C_bus_read;
	bno055->delay_msec = BNO055_delay_msek;
	bno055->dev_addr = BNO055_I2C_ADDR1;

	return BNO055_INIT_VALUE;
}

int main (int argc, char *argv[]) {
  struct bno055_t bno055;
  struct bno055_accel_t accel_xyz;
  struct bno055_gyro_t gyro_xyz;
  struct bno055_mag_t mag_xyz;

  //Begin initialization
  s32 comres = BNO055_ERROR;
  u8 power_mode = BNO055_INIT_VALUE;
  I2C_routine(&bno055);

  comres = bno055_init(&bno055);

  power_mode = BNO055_POWER_MODE_NORMAL;
  comres += bno055_set_power_mode(power_mode);
  //end initialization

  //set sensor mode to Accel. Gyro and Mag
  comres += bno055_set_operation_mode(BNO055_OPERATION_MODE_AMG);

  //read raw data
  while(1) {
    comres += bno055_read_accel_xyz(&accel_xyz);
    comres += bno055_read_mag_xyz(&mag_xyz);
    comres += bno055_read_gyro_xyz(&gyro_xyz);

    printf("accel x: %d y: %d z: %d\n", accel_xyz.x, accel_xyz.y, accel_xyz.z);
    printf("gyro x: %d y: %d z: %d\n", gyro_xyz.x, gyro_xyz.y, gyro_xyz.z);
    printf("mag x: %d y: %d z: %d\n", mag_xyz.x, mag_xyz.y, mag_xyz.z);
    sleep(1);
  }

  return comres;
}
