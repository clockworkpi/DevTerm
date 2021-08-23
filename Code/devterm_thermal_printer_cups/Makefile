CC = gcc 
BIN = rastertocpi

all:
	${CC} rastertocpi.c -o ${BIN} -I /usr/include `cups-config --cflags` `cups-config --image --libs`

debug:
	${CC} rastertocpi.c -o ${BIN} -DSAFEDEBUG -I /usr/include `cups-config --cflags` `cups-config --image --libs`

#sudo make -B install
install:
	/etc/init.d/cups stop
	cp rastertocpi /usr/lib/cups/filter/
	mkdir -p /usr/share/cups/model/clockworkpi
	cp cpi58.ppd /usr/share/cups/model/clockworkpi/
	cd /usr/lib/cups/filter
	chmod 755 rastertocpi
	chown root:root rastertocpi
	cd -
	/etc/init.d/cups start
