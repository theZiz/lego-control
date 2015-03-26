void* sensor_loop(void * ptr)
{	/*
	int socket = *((int*)ptr);

	//Sensor 1, Ev3 touch sensor
	//Sensor 2, NXT touch sensor
	int analog_file;
	ANALOG *pAnalog;
	if((analog_file = open(ANALOG_DEVICE_NAME, O_RDWR | O_SYNC)) != -1)
	{
		pAnalog = (ANALOG*)mmap(0, sizeof(ANALOG), PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, analog_file, 0);
		if (pAnalog == MAP_FAILED)
		{
			printf("Mapping analog device failed\n");
			close(analog_file);
			analog_file = -1;
		}
		else
			printf("Analog device ready\n");
	}

	//Sensor 3, NXT sound sensor
	int iic_file;
	IIC *pIic;
	if ((iic_file = open(IIC_DEVICE_NAME, O_RDWR | O_SYNC)) != -1)
	{
		pIic = (IIC*)mmap(0, sizeof(UART), PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, iic_file, 0);
		if (pAnalog == MAP_FAILED)
		{
			printf("Mapping iic device failed\n");
			close(iic_file);
			iic_file = -1;
		}
		else
			printf("Iic device ready\n");
	}
	//Sensor 4, Ev3 color sensor
	int iic_file;
	IIC *pIic;
	if ((iic_file = open(IIC_DEVICE_NAME, O_RDWR | O_SYNC)) != -1)
	{
		pIic = (IIC*)mmap(0, sizeof(UART), PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, iic_file, 0);
		if (pAnalog == MAP_FAILED)
		{
			printf("Mapping iic device failed\n");
			close(iic_file);
			iic_file = -1;
		}
		else
			printf("Iic device ready\n");
	}*/

	while (finish == 0)
	{/*
		//Reading sensor values:
		int send_data[4];
		if (analog_file != -1)
		{
			send_data[0] = (unsigned char)pAnalog->Pin6[0][pAnalog->Actual[0]];
			send_data[1] = (unsigned char)pAnalog->Pin6[1][pAnalog->Actual[1]]; //If in doubt, try Pin1
		}
		else
		{
			send_data[0] = 0;
			send_data[1] = 0;
		}
		if (iic_file != -1)
			send_data[2] = (unsigned char)pIic->Raw[2][pIic->Actual[2]][0];
		else
			send_data[2] = 0;

		send(socket, send_data, sizeof(int)*4, 0);*/
		usleep(20000);
	}
	/*if (analog_file != -1)
		close(analog_file);
	if (iic_file != -1)
		close(iic_file);*/
}
