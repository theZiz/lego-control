CFLAGS = -g -fsingle-precision-constant

LIB = ../../ev3c/lib
INCLUDE = -I../../ev3c

all: control_server

control_server: control_server.c motor.c sensor.c
	$(CC) $(CFLAGS) $(INCLUDE) control_server.c -o control_server -pthread $(LIB)/ev3c.a

clean:
	rm -f *.o
	rm -f control_server
