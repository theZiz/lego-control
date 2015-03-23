#include <fcntl.h>
#include <string.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#include <lms2012.h>
#include <d_lcd.h>
// Motor speed  0..100
const int MOTOR_SPEED = 100; //[count/s]/10
// The motor operations use a single bit (or a combination of them) 
// to determine which motor(s) will be used
// A = 0x01, B = 0x02, C = 0x04, D = 0x08
// AC = 0x05
const char MOTOR_A = 1;
const char MOTOR_B = 2;
const char MOTOR_C = 4;
const char MOTOR_D = 8;
const char MOTOR_PORT = 0x1;

char* get_ip()
{
	int fd;
	struct ifreq ifr;
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, "wlan0", IFNAMSIZ-1);
	ioctl(fd, SIOCGIFADDR, &ifr);
	close(fd);
	return inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
}

void show_ip()
{
	//Create a structure that will store the LCD information	
	LCD my_lcd;
	//Initialize and clear screen
	dLcdInit(my_lcd.Lcd);
	LCDClear(my_lcd.Lcd);
	//Draw and write something
	dLcdDrawText(my_lcd.Lcd, FG_COLOR, 1, 10, LARGE_FONT, (signed char *)"IP of Ev3:");
	dLcdDrawText(my_lcd.Lcd, FG_COLOR, 1, 35, LARGE_FONT, get_ip());
	printf("IP of Ev3: %s\n",get_ip());
	//Update screen with information
	dLcdUpdate(&my_lcd);
	//Exit
	dLcdExit();	
}

int motor_file;

void motor_set_speed(int port,int speed)
{
	if (motor_file == -1)
		return;
	char motor_command[3];
	motor_command[0] = opOUTPUT_SPEED;
	motor_command[1] = port;
	motor_command[2] = speed;
	write(motor_file,motor_command,3);
}

void motor_start(int port)
{
	if (motor_file == -1)
		return;
	char motor_command[2];
	motor_command[0] = opOUTPUT_START;
	motor_command[1] = port;
	write(motor_file,motor_command,2);
}

void motor_stop(int port)
{
	if (motor_file == -1)
		return;
	char motor_command[2];
	motor_command[0] = opOUTPUT_STOP;
	motor_command[1] = port;
	write(motor_file,motor_command,2);
}

int power = 0;
int direction = 0;
int weapon = 0;

int main()
{
	show_ip();
	if((motor_file = open(PWM_DEVICE_NAME, O_WRONLY)) == -1)
		fprintf(stderr,"Err: No motor!\n");
	motor_set_speed(MOTOR_A,MOTOR_SPEED);
	motor_start(MOTOR_A);
	sleep(2);
	motor_stop(MOTOR_A);

	motor_stop(MOTOR_B);
	motor_stop(MOTOR_C);

	int create_socket, new_socket;
	socklen_t addrlen;
	ssize_t size;
	struct sockaddr_in address;
	if ((create_socket = socket (AF_INET, SOCK_STREAM, 0)) <= 0)
	{
		fprintf(stderr,"FATAL: No socket!\n");
		return -1;
	}
	const int y = 1;
	setsockopt( create_socket, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons (15000);
	if (bind ( create_socket,
						 (struct sockaddr *) &address,
						 sizeof (address)) != 0) {
		fprintf(stderr,"FATAL: Port blocked!\n");
		return -1;
	}
	listen (create_socket, 5);
	addrlen = sizeof (struct sockaddr_in);
	new_socket = accept ( create_socket,
		(struct sockaddr *) &address,
		&addrlen );
	if (new_socket > 0)
		printf ("Ein Client (%s) ist verbunden ...\n",
	inet_ntoa (address.sin_addr));
	int message;
	do {
		size = recv (new_socket, &message, sizeof(int), 0);
		//send(new_socket, buffer, strlen (buffer), 0);
		if( size > 0)
		{
			int op = message & 65535;
			int status = message >> 16;
			printf ("Nachricht empfangen: %i %i\n", op, status);
			switch (op)
			{
				case 0:
					power = status;
					break;
				case 1:
					direction = status;
					break;
				case 2:
					weapon = status;
					break;
			}
			printf("p: %i d: %i w: %i\n",power,direction,weapon);
			if (weapon == 1)
			{
				motor_set_speed(MOTOR_A,MOTOR_SPEED);
				motor_start(MOTOR_A);
			}
			else
			if (weapon == -1)
			{
				motor_set_speed(MOTOR_A,-MOTOR_SPEED);
				motor_start(MOTOR_A);
			}
			else
				motor_stop(MOTOR_A);

			if (power && !direction)
			{
				if (power == 1)
				{
					motor_set_speed(MOTOR_B,MOTOR_SPEED);
					motor_set_speed(MOTOR_C,MOTOR_SPEED);
					motor_start(MOTOR_B);
					motor_start(MOTOR_C);
				}
				else
				{
					motor_set_speed(MOTOR_B,-MOTOR_SPEED);
					motor_set_speed(MOTOR_C,-MOTOR_SPEED);
					motor_start(MOTOR_B);
					motor_start(MOTOR_C);
				}
			}
			else
			if (!power && direction)
			{
				if (direction == 1)
				{
					motor_set_speed(MOTOR_B,MOTOR_SPEED);
					motor_set_speed(MOTOR_C,-MOTOR_SPEED);
					motor_start(MOTOR_B);
					motor_start(MOTOR_C);
				}
				else
				{
					motor_set_speed(MOTOR_B,-MOTOR_SPEED);
					motor_set_speed(MOTOR_C,MOTOR_SPEED);
					motor_start(MOTOR_B);
					motor_start(MOTOR_C);
				}
			}
			else
			if (power == 1 && direction)
			{
				if (direction == -1)
				{
					motor_set_speed(MOTOR_C,MOTOR_SPEED);
					motor_stop(MOTOR_B);
					motor_start(MOTOR_C);
				}
				else
				{
					motor_set_speed(MOTOR_B,MOTOR_SPEED);
					motor_start(MOTOR_B);
					motor_stop(MOTOR_C);
				}
			}
			else
			if (power == -1 && direction)
			{
				if (direction == -1)
				{
					motor_set_speed(MOTOR_C,-MOTOR_SPEED);
					motor_stop(MOTOR_B);
					motor_start(MOTOR_C);
				}
				else
				{
					motor_set_speed(MOTOR_B,-MOTOR_SPEED);
					motor_start(MOTOR_B);
					motor_stop(MOTOR_C);
				}
			}
			else
			{
				motor_stop(MOTOR_B);
				motor_stop(MOTOR_C);				
			}
		}
		else
			break;
	} while (message != -1);
	close (new_socket);
	close (create_socket);
 
	if (motor_file != -1)
		close(motor_file);
	return 0;
}
