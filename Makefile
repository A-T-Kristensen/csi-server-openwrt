CC = mips-openwrt-linux-gcc
CC86 = gcc
CFLAGS = -Wall -fno-builtin-log

OBJ = csi_tool.o main.o csi_service.o logger.o server.o core.o util.o

all: $(OBJ)
	$(CC) $(CFLAGS) -o prog $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

x86: csi_tool.c csi_tool.h main.c csi_service.c csi_service.h logger.c logger.h server.c server.h core.c core.h util.c util.h
	$(CC86) $(CFLAGS) -o progx86 csi_tool.c csi_tool.h main.c csi_service.c csi_service.h logger.c logger.h server.c server.h core.c core.h util.c util.h -std=c99

clean:
	rm -f *.o prog progx86
