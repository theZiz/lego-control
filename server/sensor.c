void* sensor_loop(void * ptr)
{	
	int socket = *((int*)ptr);
	
	ev3_sensor_ptr sensors = ev3_load_sensors();
	ev3_driver_sensor(
		ev3_search_sensor_by_identifier( sensors, NXT_ANALOG, 0 ),
		"lego-nxt-sound"
	);
	ev3_sensor_ptr sensor = sensors;
	while (sensor)
	{
		ev3_mode_sensor( sensor, 0 );
		ev3_open_sensor( sensor );
		sensor = sensor->next;
	}
	//Sending the names of the four sensors
	int i;
	sensor = sensors;
	char name[128];
	for (i = 0; i < 4 && sensor; i++)
	{
		sprintf(name,"%s",sensor->driver_name);
		send(socket, name, sizeof(char)*128, 0);
		sensor = sensor->next;
	}
	for (; i < 4; i++)
	{
		sprintf(name,"not connected");
		send(socket, name, sizeof(char)*128, 0);
	}

	int c = 0;
	while (finish == 0)
	{
		int32_t send_data[4];
		sensor = sensors;
		for (i = 0; i < 4 && sensor; i++)
		{
			ev3_update_sensor_val( sensor );
			send_data[i] = sensor->val_data[0].s32;
			sensor = sensor->next;
		}
		send(socket, send_data, sizeof(int32_t)*4, 0);
		usleep(20000);
	}
	ev3_delete_sensors(sensors);
}
