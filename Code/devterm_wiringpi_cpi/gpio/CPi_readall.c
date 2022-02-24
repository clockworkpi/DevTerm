#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include "CPi.h"

#ifdef CONFIG_CLOCKWORKPI_A04

int bcmToGpioCPi[64] =
{
	58,  57,	  // 0, 1
	167, 0, 	 // 2, 3
	1, 2,	   // 4  5
	3,	4,		// 6, 7
	5,	6,		// 8, 9
	7,	8,		//10,11
	15,  54,	  //12,13
	134,  135,		//14,15

	137, 136,	   //16,17
	139,  138,		//18,19
	141,  140,		//20,21
	128,  129,		//22,23
	130,  131,		//24,25
	132, 133,	   //26,27
	9,	201,	//28,29
	196,   199,    //30,31

	161,  160,		//32,33
	227,  198,		//34,35
	163, 166,	   //36,37
	165,  164,		//38,39
	228,  224,		//40,41
	225, 226,	   //42,43
	56,  55,	  //44,45
	-1, -1, 	 //46,47

	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,// ... 63
};

#endif

#ifdef CONFIG_CLOCKWORKPI_A06

int bcmToGpioCPi[64] =
{
	106,  107,      // 0, 1
	104, 10,      // 2, 3
	3, 9,      // 4  5
	4,  90,      // 6, 7
	92,  158,      // 8, 9
	156,  105,      //10,11
	146,  150,      //12,13
	81,  80,      //14,15

	82, 83,      //16,17
	131,  132,      //18,19
	134,  135,      //20,21
	89,  88,      //22,23
	84,  85,      //24,25
	86, 87,      //26,27
	112,  113,    //28,29
	109,   157,    //30,31

	148,  147,      //32,33
	100,  101,      //34,35
	102, 103,      //36,37
	97,  98,      //38,39
	99,  96,      //40,41
	110, 111,      //42,43
	64,  65,      //44,45
	-1, -1,      //46,47

	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,// ... 63
};

#endif

static char *alts [] =
{
  "IN", "OUT", "ALT2", "ALT3", "ALT4", "ALT5", "ALT6", "OFF"
} ;

static char* get_pin_name(int pin)
{
	static char name[10];
	char c;
	int b, d;

	b = pin/32;
	d = pin % 32;

#ifdef CONFIG_CLOCKWORKPI_A04

	if(b < 6)
		c = b + 'C';
	else
		c = b - 6 + 'L';
	sprintf(name, "P%c%d", c, d);

#elif defined(CONFIG_CLOCKWORKPI_A06)

	c = d/8 + 'A';
	sprintf(name, "%d%c%d", b, c, d % 8);

#endif
	return name;
}

void CPiReadAll(void)
{
	int pin, pin2;
	int tmp = wiringPiDebug;
	wiringPiDebug = FALSE;

	printf (" +-----+------+------+------+---+-----+------+------+------+---+\n");
	printf (" | BCM | GPIO | Name | Mode | V | BCM | GPIO | Name | Mode | V |\n");
	printf (" +-----+------+------+------+---+-----+------+------+------+---+\n");

	for (pin = 0 ; pin < 23; pin ++) {
		printf (" | %3d", pin);
		printf (" | %4d", bcmToGpioCPi[pin]);
		printf (" | %-4s", get_pin_name(bcmToGpioCPi[pin]));
		printf (" | %4s", alts [CPi_get_gpio_mode(bcmToGpioCPi[pin])]);
		printf (" | %d", CPi_digitalRead(bcmToGpioCPi[pin])) ;
		pin2 = pin + 23;
		printf (" | %3d", pin2);
		printf (" | %4d", bcmToGpioCPi[pin2]);
		printf (" | %-4s", get_pin_name(bcmToGpioCPi[pin2]));
		printf (" | %4s", alts [CPi_get_gpio_mode(bcmToGpioCPi[pin2])]);
		printf (" | %d", CPi_digitalRead(bcmToGpioCPi[pin2])) ;
		printf (" |\n") ;
	}

	printf (" +-----+------+------+------+---+-----+------+------+------+---+\n");
	printf (" | BCM | GPIO | Name | Mode | V | BCM | GPIO | Name | Mode | V |\n");
	printf (" +-----+------+------+------+---+-----+------+------+------+---+\n");

	wiringPiDebug = tmp;
}

void CPiReadAllRaw(void)
{
	int pin, pin2, i;
	int tmp = wiringPiDebug;
	wiringPiDebug = FALSE;

#ifdef CONFIG_CLOCKWORKPI_A04

	printf (" +------+------+------+---+------+------+------+---+------+------+------+---+------+------+------+---+------+------+------+---+------+------+------+---+\n");
	printf (" | GPIO | Name | Mode | V | GPIO | Name | Mode | V | GPIO | Name | Mode | V | GPIO | Name | Mode | V | GPIO | Name | Mode | V | GPIO | Name | Mode | V |\n");
	printf (" +------+------+------+---+------+------+------+---+------+------+------+---+------+------+------+---+------+------+------+---+------+------+------+---+\n");

	for (pin = 0 ; pin < 27; pin++) {
		pin2 = pin;
		for(i = 0; i < 6; i++) {
			if(CPi_get_gpio_mode(pin2) >= 0) {
				printf (" | %4d", pin2) ;
				printf (" | %-4s", get_pin_name(pin2));
				printf (" | %4s", alts [CPi_get_gpio_mode(pin2)]) ;
				printf (" | %d", CPi_digitalRead(pin2)) ;
			} else {
				printf (" |     ") ;
				printf (" |     ") ;
				printf (" |     ") ;
				printf (" |  ") ;
			}
			pin2 += 32;
			if(i == 1) pin2 += 64;
		}
		printf (" |\n") ;
	}

	printf (" +------+------+------+---+------+------+------+---+------+------+------+---+------+------+------+---+------+------+------+---+------+------+------+---+\n");
	printf (" | GPIO | Name | Mode | V | GPIO | Name | Mode | V | GPIO | Name | Mode | V | GPIO | Name | Mode | V | GPIO | Name | Mode | V | GPIO | Name | Mode | V |\n");
	printf (" +------+------+------+---+------+------+------+---+------+------+------+---+------+------+------+---+------+------+------+---+------+------+------+---+\n");

#elif defined(CONFIG_CLOCKWORKPI_A06)

	printf (" +------+------+---+------+------+---+------+------+---+------+------+---+------+------+---+\n");
	printf (" | GPIO | Mode | V | GPIO | Mode | V | GPIO | Mode | V | GPIO | Mode | V | GPIO | Mode | V |\n");
	printf (" +------+------+---+------+------+---+------+------+---+------+------+---+------+------+---+\n");

	for (pin = 0 ; pin < 32; pin++) {
		pin2 = pin;
		for(i = 0; i < 5; i++) {
			printf (" | %4d", pin2) ;
			printf (" | %4s", alts [CPi_get_gpio_mode(pin2)]) ;
			printf (" | %d", CPi_digitalRead(pin2)) ;
			pin2 += 32;
		}
		printf (" |\n") ;
	}

	printf (" +------+------+---+------+------+---+------+------+---+------+------+---+------+------+---+\n");
	printf (" | GPIO | Mode | V | GPIO | Mode | V | GPIO | Mode | V | GPIO | Mode | V | GPIO | Mode | V |\n");
	printf (" +------+------+---+------+------+---+------+------+---+------+------+---+------+------+---+\n");

#endif

	wiringPiDebug = tmp;
}

