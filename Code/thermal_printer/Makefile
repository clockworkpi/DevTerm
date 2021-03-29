CC = gcc

CFLAGS  = -g -Wall
LDFLAUS = 
INCLUDES = 
LIBS = -lwiringPi

MAIN = devterm_thermal_printer.elf

SRCS = printer.c  devterm_thermal_printer.c  utils.c
OBJS = $(SRCS:.c=.o)

.PHONY: depend clean

all:    $(MAIN)
	@echo compile $(MAIN)

$(MAIN): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)
        

