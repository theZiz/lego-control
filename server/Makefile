CFLAGS = -O3 -fsingle-precision-constant

all: control_server

control_server: control_server.c motor.c sensor.c graphics.c
	$(CC) $(CFLAGS) control_server.c -o control_server -pthread

clean:
	rm -f *.o
	rm -f control_server
