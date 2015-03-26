#include <netinet/in.h>
#include <sparrow3d.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>

SDL_Surface* screen;
spFontPointer font;
char *target;
int create_socket;

#define BUFFER_SIZE 1024
int sensor_data[BUFFER_SIZE][4];
int sensor_ptr = 0;

void draw(void)
{
	spClearTarget(0);
	spFontDrawMiddle( screen->w >> 2, 2, 0, "Ev3 Control App", font );
	char buffer[256];
	sprintf(buffer,"to IP %s",target);
	spFontDrawMiddle( screen->w >> 2, 1 * font->maxheight + 2, 0, buffer, font );
	spFontDraw( 2, 3 * font->maxheight + 2, 0, "[^] Forward", font );
	spFontDraw( 2, 4 * font->maxheight + 2, 0, "[v] Backward", font );
	spFontDraw( 2, 5 * font->maxheight + 2, 0, "[<] Left", font );
	spFontDraw( 2, 6 * font->maxheight + 2, 0, "[>] Right", font );
	spFontDraw( 2, 7 * font->maxheight + 2, 0, "[q] Weapon forward", font );
	spFontDraw( 2, 8 * font->maxheight + 2, 0, "[e] Weapon backward", font );
	spFontDraw( 2, 10 * font->maxheight + 2, 0, "[s] Exit on Ev3", font );
	spFontDraw( 2, screen->h - font->maxheight - 2, 0, "[R] Exit", font );
	spLine( screen->w >> 1,0,0,screen->w >> 1,screen->h,0,65535);
	int i;
	for (i = 0; i < 4; i++)
	{
		char buffer[32];
		switch (i)
		{
			case 0: sprintf(buffer,"1 (Ev3 touchsensor):"); break;
			case 1: sprintf(buffer,"2 (NXT touchsensor):"); break;
			case 2: sprintf(buffer,"3 (Soundsensor):"); break;
			case 3: sprintf(buffer,"4 (Colorsensor):"); break;
		}
		int y = (i+1)*screen->h >> 2;
		int height = screen->h >> 2;
		spFontDraw( screen->w + 8 >> 1, i*screen->h + 8 >> 2, 0, buffer, font );
		spLine( screen->w >> 1,y,0, screen->w,y,0,65535);
		int x;
		y--;
		//find max
		int max = 1;
		int ptr = sensor_ptr;
		for (x = screen->w-1; x > (screen->w >> 1); x--)
		{
			if (sensor_data[ptr][i] > max)
				max = sensor_data[ptr][i];
			ptr--;
			if (ptr < 0)
				ptr = BUFFER_SIZE-1;
		}
		ptr = sensor_ptr;
		for (x = screen->w-1; x > (screen->w >> 1); x--)
		{
			spLine( x,y,0, x,y-sensor_data[ptr][i]*height/max,0,spGetFastRGB(255,255,0));
			ptr--;
			if (ptr < 0)
				ptr = BUFFER_SIZE-1;
		}
	}
	spFlip();
}

int power = 0;
int direction = 0;
int weapon = 0;

void send_message(int message,int status)
{
	printf("Sending %i: %i\n",message,status);
	int m = message + (status << 16);
	send(create_socket, &m, sizeof(int), 0);
}

int finish = 0;

int calc(Uint32 steps)
{
	if (spGetInput()->axis[1] < 0)
	{
		if (power != 1)
			send_message(0,1);
		power = 1;
	}
	else
	if (spGetInput()->axis[1] > 0)
	{
		if (power != -1)
			send_message(0,-1);
		power = -1;
	}
	else
	{
		if (power != 0)
			send_message(0,0);
		power = 0;
	}
	if (spGetInput()->axis[0] > 0)
	{
		if (direction != 1)
			send_message(1,1);
		direction = 1;
	}
	else
	if (spGetInput()->axis[0] < 0)
	{
		if (direction != -1)
			send_message(1,-1);
		direction = -1;
	}
	else
	{
		if (direction != 0)
			send_message(1,0);
		direction = 0;
	}
	if (spGetInput()->button[SP_BUTTON_R])
	{
		if (weapon != 1)
			send_message(2,1);
		weapon = 1;
	}
	else
	if (spGetInput()->button[SP_BUTTON_L])
	{
		if (weapon != -1)
			send_message(2,-1);
		weapon = -1;
	}
	else
	{
		if (weapon != 0)
			send_message(2,0);
		weapon = 0;
	}
	if (spGetInput()->button[SP_BUTTON_X])
	{
		spGetInput()->button[SP_BUTTON_X] = 0;
		send_message(-1,0);
	}
	if (spGetInput()->button[SP_BUTTON_START])
		return 1;
	return 0;
}

void resize( Uint16 w, Uint16 h )
{
	spSelectRenderTarget(screen);
	//Font Loading
	if ( font )
		spFontDelete( font );
	spFontSetShadeColor(0);
	font = spFontLoad( "./DejaVuSans-Bold.ttf",16 * spGetSizeFactor() >> SP_ACCURACY);
	spFontAdd( font, SP_FONT_GROUP_ASCII, 65535 ); //whole ASCII
	spFontAddButton( font, 'R', SP_BUTTON_START_NAME, 65535, spGetRGB(127,127,127) ); //Return == START
	spFontAddButton( font, 'B', SP_BUTTON_SELECT_NOWASD_NAME, 65535, spGetRGB(127,127,127) ); //Backspace == SELECT
	spFontAddButton( font, 'q', SP_BUTTON_L_NAME, 65535, spGetRGB(127,127,127) ); // q == L
	spFontAddButton( font, 'e', SP_BUTTON_R_NAME, 65535, spGetRGB(127,127,127) ); // e == R
	spFontAddButton( font, 'a', SP_BUTTON_LEFT_NAME, 65535, spGetRGB(127,127,127) ); //a == left button
	spFontAddButton( font, 'd', SP_BUTTON_RIGHT_NAME, 65535, spGetRGB(127,127,127) ); // d == right button
	spFontAddButton( font, 'w', SP_BUTTON_UP_NAME, 65535, spGetRGB(127,127,127) ); // w == up button
	spFontAddButton( font, 's', SP_BUTTON_DOWN_NAME, 65535, spGetRGB(127,127,127) ); // s == down button
	spFontAddArrowButton( font, '<', SP_BUTTON_ARROW_LEFT, 65535, spGetRGB(127,127,127) );
	spFontAddArrowButton( font, '^', SP_BUTTON_ARROW_UP, 65535, spGetRGB(127,127,127) );
	spFontAddArrowButton( font, '>', SP_BUTTON_ARROW_RIGHT, 65535, spGetRGB(127,127,127) );
	spFontAddArrowButton( font, 'v', SP_BUTTON_ARROW_DOWN, 65535, spGetRGB(127,127,127) );
	
	spFontMulWidth(font,spFloatToFixed(0.85f));
	spFontAddBorder(font , 0);
}

void* sensor_loop(void * ptr)
{	
	int socket = *((int*)ptr);
	int sensor_1;
	while (finish == 0)
	{
		//Reading sensor values:
		ssize_t size;
		int n_ptr = sensor_ptr+1;
		if (n_ptr >= BUFFER_SIZE)
			n_ptr = 0;
		size = recv (socket, &(sensor_data[n_ptr]), sizeof(int)*4, 0);
		sensor_ptr = n_ptr;
	}
}

int main(int argc, char **argv)
{
	if (argc != 2)
		return 1;
	memset( sensor_data, 0, BUFFER_SIZE*4*sizeof(int));
	target = argv[1];
	spInitCore();
	spSetDefaultWindowSize( 800, 480 );
	screen = spCreateDefaultWindow();
	spSetZSet(0);
	spSetZTest(0);
	resize( screen->w, screen->h );
	struct sockaddr_in address;
	int size;
	if ((create_socket=socket (AF_INET, SOCK_STREAM, 0)) > 0)
		printf ("Socket wurde angelegt\n");
	address.sin_family = AF_INET;
	address.sin_port = htons (15000);
	inet_aton (target, &address.sin_addr);
	if (connect ( create_socket,
		(struct sockaddr *) &address,
		sizeof (address)) == 0)
		printf ("Verbindung mit dem Server (%s) hergestellt\n",
			inet_ntoa (address.sin_addr));
	pthread_t sensor_thread;
	pthread_create( &sensor_thread, NULL, sensor_loop, &create_socket);
	spLoop(draw,calc,10,resize,NULL);
	finish = 1;
	pthread_exit( sensor_thread, NULL);
	close (create_socket);
	spQuitCore();
	return 0;
}
