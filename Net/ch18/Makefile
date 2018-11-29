CC=gcc
TARGET=1122sm
OBJECT=sm1122.o

$(TARGET) : $(OBJECT)
	rm $(TARGET)
	$(CC) -O $(TARGET) sm1122.o
	rm $(OBJECT)

sm1122.o : sm1122.c
	$(CC) -c -o sm1122.o sm1122.c

