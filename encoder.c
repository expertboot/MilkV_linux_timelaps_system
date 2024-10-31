#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "wiringx.h"
#include <pthread.h>
//#include <sys/syscall.h>
#include "encoder.h"

pthread_t pth;

typedef struct {
    int gpio1;
    int gpio2;
    int value;
	int running;
} ThreadData;

ThreadData data;


void *threadEncoder(void *arg) {

    ThreadData *data = (ThreadData *)arg; // Приведение типа
    int gpio1 = data->gpio1;
    int gpio2 = data->gpio2;
    uint8_t pinstate = 0;
    uint8_t encState = 0;
	while(data->running)
   	{
		pinstate = digitalRead(gpio1) | (digitalRead(gpio2) << 1); 
		switch(encState)
			{
			case 2:
				{
				if(pinstate== 3) data->value++;
				if(pinstate == 0) data->value--; 
				break;
				}
		 
			case 0:
				{
				if(pinstate == 2) data->value++;
				if(pinstate == 1) data->value--; 
				break;
				}
			case 1:
				{
				if(pinstate == 0) data->value++;
				if(pinstate == 3) data->value--; 
				break;
				}
			case 3:
				{
				if(pinstate == 1) data->value++;
				if(pinstate == 2) data->value--; 
				break;
				}
			}
		 
		encState = pinstate; 
		usleep(300);
	};

    printf("END THREAD ENCODER SCAN\n");
    sleep(1);
    return 0;
}

int encoderInit(int gpio_a, int gpio_b)
{
	data.gpio1 = gpio_a;
	data.gpio2 = gpio_b;
	data.value = 0;
	data.running = 1;

	pinMode(gpio_a, PINMODE_INPUT);
	pinMode(gpio_b, PINMODE_INPUT);	
	int err = pthread_create(&pth, NULL, threadEncoder, (void *)&data);
    if(err != 0) {
        printf("ERR create thread encoder scan: [%s]\n", strerror(err));
        wiringXGC();
        return -1;
	}else{
		printf("pthread ok \n");
	}
	return 0;
};

int encoderRead(void)
{
	return data.value/4;
}

void encoderClose(void)
{
	data.running = 0;
	pthread_join(pth, NULL);
}

void encoderWrite(int value)
{
	data.value = value * 4;
}



