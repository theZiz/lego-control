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

void draw(void)
{
	spClearTarget(0);
	spFontDrawMiddle( screen->w >> 1, 2, 0, "Ev3 Control App", font );
	char buffer[256];
	sprintf(buffer,"to IP %s",target);
	spFontDrawMiddle( screen->w >> 1, 1 * font->maxheight + 2, 0, buffer, font );
	spFontDrawMiddle( screen->w >> 1, 3 * font->maxheight + 2, 0, "^ Forward", font );
	spFontDrawMiddle( screen->w >> 1, 4 * font->maxheight + 2, 0, "v Backward", font );
	spFontDrawMiddle( screen->w >> 1, 5 * font->maxheight + 2, 0, "< Left", font );
	spFontDrawMiddle( screen->w >> 1, 6 * font->maxheight + 2, 0, "> Right", font );
	spFontDrawMiddle( screen->w >> 1, 7 * font->maxheight + 2, 0, "[q] Weapon forward", font );
	spFontDrawMiddle( screen->w >> 1, 8 * font->maxheight + 2, 0, "[e] Weapon backward", font );
	spFontDrawMiddle( screen->w >> 1, 10 * font->maxheight + 2, 0, "[s] Exit on Ev3", font );
	spFontDrawMiddle( screen->w >> 1, screen->h - font->maxheight - 2, 0, "[R] Exit", font );
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
	spFontMulWidth(font,spFloatToFixed(0.85f));
	spFontAddBorder(font , 0);
}

int main(int argc, char **argv)
{
	target = argv[1];
	spInitCore();
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
	spLoop(draw,calc,10,resize,NULL);
	close (create_socket);
	spQuitCore();
	return 0;
}
