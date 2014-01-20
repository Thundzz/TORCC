TORCS_INC               = include
TARGET                  = lib/enseirBOT.so
CPPFLAGS        = -I$(TORCS_INC) -Wall
LDFLAGS         = -fPIC

all:$(TARGET)

lib/enseirBOT.so:objs/mydrive.o objs/driver.o
	gcc -shared -o $@ $^ -fPIC
objs/%.o:src/%.cpp
	g++ $(CPPFLAGS) -fPIC -c -o $@ $^

objs/%.o:src/%.ll
	llc -filetype=obj -relocation-model=pic -o $@ $^

install:$(TARGET)
	cp $^ ../../../res/lib/enseirBOT/enseirBOT.so

cp:$(TARGET)
	cp $^ /usr/local/lib/torcs/drivers/enseirBOT

clean:
	rm lib/* objs/*

