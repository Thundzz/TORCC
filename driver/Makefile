TORCS_INC		= /home/bertrand/Downloads/torcs-1.3.5/export/include/
TARGET			= lib/enseirBOT.so
TORCS_INSTALL	= $(HOME)/local/

CPPFLAGS	= -I$(TORCS_INC) -Wall
LDFLAGS		= -fPIC 

all:$(TARGET)

lib/enseirBOT.so:objs/drive.o objs/driver.o
	gcc -shared -Wl,-soname,-enseirBOT $^ -o $@ $(LDFLAGS)

objs/%.o:src/%.cpp
	clang++ $(CPPFLAGS) -c -o $@ $^ $(LDFLAGS)

objs/%.o:src/%.ll
	clang -c -o $@ $^ $(LDFLAGS)

install:$(TARGET)
	cp $^ $(TORCS_INSTALL)lib/torcs/drivers/enseirBOT/enseirBOT.so

clean:
	rm lib/* objs/*
