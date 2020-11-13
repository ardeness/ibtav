CC=gcc
OBJS=ibtav.c cpu.c inst.c elf.c symbol.c relocation.c jump.c createcpu.c modifyinst.c
BIN=$(HOME)/bin/
TARGET=$(BIN)ibtav
TARGET_DEBUG=$(BIN)ibtav_debug

all : $(OBJS)
	$(CC) $(OBJS) -fno-function-cse -o $(TARGET)

debug: $(OBJS)
	$(CC) -g -DDEBUG $(OBJS) -o $(TARGET_DEBUG)

clean:
	rm -f $(TARGET) $(TARGET_DEBUG)
	rm -f *.o
