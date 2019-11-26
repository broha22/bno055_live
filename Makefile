PI_ADDRESS = 192.168.1.132

remote_run: remote
	ssh pi@$(PI_ADDRESS) 'bno055_live/sensor_read'

remote:
	npm --prefix=client/ run build
	rsync -av -e ssh --exclude='.git/' --exclude='*/node_modules/' ./ pi@$(PI_ADDRESS):~/bno055_live
	ssh pi@$(PI_ADDRESS) 'cd bno055_live; make all; cd server; npm install;'

all: rpi_bno055_socket.o bno055.o
	gcc -Wall bno055.o rpi_bno055_socket.o -o sensor_read
 
rpi_bno055_socket.o: rpi_bno055_socket.c lib/BNO055_driver/bno055.h
	gcc -c rpi_bno055_socket.c

bno055.o: lib/BNO055_driver/bno055.c lib/BNO055_driver/bno055.h
	gcc -c lib/BNO055_driver/bno055.c

clean:
	rm -rf *.o
	rm rpi_bno055_socket