#ifndef _CPI_H_
#define _CPI_H_

extern int wiringPiSetupRaw (void);
extern void CPiBoardId (int *model, int *rev, int *mem, int *maker, int *warranty);
extern int CPi_get_gpio_mode(int pin);
extern int CPiDigitalRead(int pin);
extern void CPiDigitalWrite(int pin, int value);
extern void CPiReadAll(void);
extern void CPiReadAllRaw(void);

#endif
