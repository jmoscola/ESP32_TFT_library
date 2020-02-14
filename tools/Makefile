
CC = gcc
TFT=../components/tft
INCLUDES=-I$(TFT)
CFLAGS = -Wall
TARGET = c2fon

all: $(TARGET)

tftfontfiles.o: $(TFT)/tftfontfiles.h $(TFT)/tftfontfiles.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $(TFT)/tftfontfiles.c

$(TARGET): tftfontfiles.o $(TARGET).c
	$(CC) $(CFLAGS) $(INCLUDES) $(TARGET).c -o $(TARGET) *.o 

clean:
	$(RM) *.o $(TARGET)
