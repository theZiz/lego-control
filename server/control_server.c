#include <fcntl.h>
#include <string.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#include <pthread.h>
#include <ev3c.h>

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
	ev3_clear_lcd();
	int i;
	for (i = 0; i < 4;i++)
		ev3_circle_lcd(rand()%(int)EV3_X_LCD,rand()%(int)EV3_Y_LCD,rand()%((int)EV3_Y_LCD/2),rand()%2);
	for (i = 0; i < 100;i++)
		ev3_line_lcd(rand()%(int)EV3_X_LCD,rand()%(int)EV3_Y_LCD,rand()%(int)EV3_X_LCD,rand()%(int)EV3_Y_LCD,rand()%2);
	/*
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
	dLcdExit();*/
}

int finish = 0;

#include "sensor.c"
#include "motor.c"

int main()
{
	ev3_init_lcd();
	show_ip();
	srand(time(NULL));
	int create_socket, new_socket;
	socklen_t addrlen;
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
						 sizeof (address)) != 0)
	{
		fprintf(stderr,"FATAL: Port blocked!\n");
		return -1;
	}
	listen (create_socket, 5);
	addrlen = sizeof (struct sockaddr_in);
	new_socket = accept ( create_socket,
		(struct sockaddr *) &address,
		&addrlen );
	if (new_socket > 0)
		printf ("Client (%s) is connected ...\n",
	inet_ntoa (address.sin_addr));
	pthread_t motor_thread;
	pthread_t sensor_thread;
	pthread_create( &motor_thread, NULL, motor_loop, &new_socket);
	pthread_create( &sensor_thread, NULL, sensor_loop, &new_socket);
	
	pthread_join( motor_thread, NULL);
	pthread_join( sensor_thread, NULL);
	
	close (new_socket);
	close (create_socket);
    	
	ev3_quit_lcd();
 	return 0;
}
