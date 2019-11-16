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

#define	I2C_BUFFER_LEN 9
#define I2C0 5
#define	BNO055_I2C_BUS_WRITE_ARRAY_INDEX	((u8)1)
int file_i2c;
struct bno055_t bno055;

s8 BNO055_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt) {
  s32 BNO055_iERROR = BNO055_INIT_VALUE;
	u8 array[I2C_BUFFER_LEN];
	u8 stringpos = BNO055_INIT_VALUE;
	array[BNO055_INIT_VALUE] = reg_addr;
	for (stringpos = BNO055_INIT_VALUE; stringpos < cnt; stringpos++) {
		array[stringpos + BNO055_I2C_BUS_WRITE_ARRAY_INDEX] =
			*(reg_data + stringpos);
	}
  printf("W%x: ", reg_addr);
  for (int i = 1; i <= cnt; i++) {
    printf("%x ", array[i]);
  }
  printf("\n");

  if (write(file_i2c, array, cnt + 1) != cnt + 1) {
		printf("Failed to write to the i2c bus.\n");
	}
  
  return (s8)BNO055_iERROR;
}

s8 BNO055_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt) {
  s32 BNO055_iERROR = BNO055_INIT_VALUE;
	u8 array[I2C_BUFFER_LEN] = {BNO055_INIT_VALUE};
	u8 stringpos = BNO055_INIT_VALUE;

  if (write(file_i2c, &reg_addr, 1) != 1) {
		printf("Failed to write to the i2c bus.\n");
	}
  sleep(0.15);
  if (read(file_i2c, array, cnt) != cnt) {
		printf("Failed to read from the i2c bus.\n");
	}
  printf("R%x: ", reg_addr);
  for (int i = 0; i < cnt; i++)
    printf("%x ", array[i]);
  printf("\n");

  for (stringpos = BNO055_INIT_VALUE; stringpos < cnt; stringpos++) {
		*(reg_data + stringpos) = array[stringpos];
  }
	return (s8)BNO055_iERROR;
}

void BNO055_delay_msek(u32 msek) {
  sleep((double)msek / 1000.0);
}

s8 I2C_routine(struct bno055_t *bno055) {
	bno055->bus_write = BNO055_I2C_bus_write;
	bno055->bus_read = BNO055_I2C_bus_read;
	bno055->delay_msec = BNO055_delay_msek;
	bno055->dev_addr = BNO055_I2C_ADDR1;

  if ((file_i2c = open("/dev/i2c-1", O_RDWR)) < 0) {
    return -1;
  }
  if(ioctl(file_i2c, I2C_SLAVE, bno055->dev_addr) != 0) {
    return -1;
  }
  u8 reg = BNO055_CHIP_ID_ADDR;
  if(write(file_i2c, &reg, 1) != 1) {
    return -1;
  }

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

  s16 gyro_x, gyro_y, gyro_z = 0;
  while (1) {
    comres += bno055_read_gyro_x(&gyro_x);
    comres += bno055_read_gyro_y(&gyro_y);
    comres += bno055_read_gyro_z(&gyro_z);

    printf("%f %f %f\n", (double)gyro_x / 16.0, (double)gyro_y / 16.0, (double)gyro_z / 16.0);
    sleep(1);
  }

  return comres;
}
