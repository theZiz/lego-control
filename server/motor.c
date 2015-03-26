const char MOTOR_A = 1;
const char MOTOR_B = 2;
const char MOTOR_C = 4;
const char MOTOR_D = 8;
//int motor_file;
const int MOTOR_SPEED = 100;

void motor_set_speed(int port,int speed)
{
	/*if (motor_file == -1)
		return;
	char motor_command[3];
	motor_command[0] = opOUTPUT_POWER;
	motor_command[1] = port;
	motor_command[2] = speed;
	write(motor_file,motor_command,3);*/
}

void motor_start(int port)
{
	/*if (motor_file == -1)
		return;
	char motor_command[2];
	motor_command[0] = opOUTPUT_START;
	motor_command[1] = port;
	write(motor_file,motor_command,2);*/
}

void motor_stop(int port)
{
	/*if (motor_file == -1)
		return;
	char motor_command[2];
	motor_command[0] = opOUTPUT_STOP;
	motor_command[1] = port;
	write(motor_file,motor_command,2);*/
}

int power = 0;
int direction = 0;
int weapon = 0;

void* motor_loop(void * ptr)
{
	/*if((motor_file = open(PWM_DEVICE_NAME, O_WRONLY)) == -1)
		fprintf(stderr,"Err: No motor!\n");*/
	int message;
	int socket = *((int*)ptr);
	do
	{
		ssize_t size;
		size = recv (socket, &message, sizeof(int), 0);
		if( size > 0)
		{
			int op = message & 65535;
			int status = message >> 16;
			printf ("Got message: %i %i\n", op, status);
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
				motor_set_speed(MOTOR_A,MOTOR_SPEED/4);
				motor_start(MOTOR_A);
			}
			else
			if (weapon == -1)
			{
				motor_set_speed(MOTOR_A,-MOTOR_SPEED/4);
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
	}
	while (message != -1);
	finish = 1;
	/*if (motor_file != -1)
		close(motor_file);*/
	return NULL;
}

