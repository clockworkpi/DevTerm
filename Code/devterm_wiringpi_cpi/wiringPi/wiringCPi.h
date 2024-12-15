#ifndef _WIRING_CPI_H
#define _WIRING_CPI_H

#ifdef CONFIG_CLOCKWORKPI_A04
#define GPIOA_BASE		0x0300B000
#define GPIO_BASE_MAP	(GPIOA_BASE + 0x24 * 2)
#define GPIOL_BASE		(0x07022000)
#define GPIO_PWM_OP		(0x0300A000)
#define GPIO_NUM			(256)
#endif

#ifdef CONFIG_CLOCKWORKPI_A06
#define GPIO0_BASE						0xff720000
#define GPIO1_BASE						0xff730000
#define GPIO2_BASE						0xff780000
#define GPIO3_BASE						0xff788000
#define GPIO4_BASE						0xff790000
#define GPIO_SWPORTA_DR_OFFSET			0x00
#define GPIO_SWPORTA_DDR_OFFSET		0x04
#define GPIO_EXT_PORTA_OFFSET			0x50
#define PMUGRF_BASE						0xff320000
#define GRF_BASE							0xff77e000
#define CRU_BASE							0xff760000
#define PMUCRU_BASE						0xff750000
#define CRU_CLKGATE_CON31_OFFSET		0x037c
#define PMUCRU_CLKGATE_CON1_OFFSET		0x0104
#define GPIO_NUM							(160)

#define PMUGRF_IOMUX_0        (PMUGRF_BASE+0x00)
#define PMUGRF_IOMUX_1        (PMUGRF_BASE+0x10)
#define PMUGRF_PUD_0          (PMUGRF_BASE+0x40)
#define PMUGRF_PUD_1          (PMUGRF_BASE+0x50)

#define GRF_IOMUX_2           (GRF_BASE+0x00)
#define GRF_IOMUX_3           (GRF_BASE+0x10)
#define GRF_IOMUX_4           (GRF_BASE+0x20)
#define GRF_PUD_2             (GRF_BASE+0x40)
#define GRF_PUD_3             (GRF_BASE+0x50)
#define GRF_PUD_4             (GRF_BASE+0x60)

#endif

#define MAP_SIZE		(4*1024)
#define MAP_MASK	(MAP_SIZE - 1)

extern int wiringPiDebug;
extern int wiringPiReturnCodes;

extern void CPiPinMode(int pin, int mode);
extern int CPi_get_gpio_mode(int pin);
extern void CPiDigitalWrite(int pin, int value);
extern int CPiDigitalRead(int pin);
extern void CPiSetupRaw(void);
extern int CPiSetup(int fd);
extern void CPiBoardId (int *model, int *rev, int *mem, int *maker, int *warranty);
extern void CPiPullUpDnControl(int pin, int pud);

#endif
