PI_ADDRESS = 192.168.1.132

remote_run: remote
	ssh pi@$(PI_ADDRESS) 'bno055_live/rpi_bno055_socket'

remote:
	rsync -av -e ssh --exclude='.git/' ./ pi@$(PI_ADDRESS):~/bno055_live
	ssh pi@$(PI_ADDRESS) 'cd bno055_live; make all'

all: rpi_bno055_socket.o
 
rpi_bno055_socket.o:
	g++ rpi_bno055_socket.cpp -o rpi_bno055_socket

clean:
	rm -rf *.o
	rm rpi_bno055_socket