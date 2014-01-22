##############################################################################
#
#    file                 : Makefile
#    created              : Mon Dec 16 10:57:15 CET 2013
#    copyright            : (C) 2002 enseirb robot
#
##############################################################################
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
##############################################################################
enseirbot.so: enseirbot.o drive.o
	g++ -shared -o $@ $^ -L/net/ens/dbarthou/src/torcs-1.3.5/export/lib -L${HOME}/lib -lopenal -lalut -lvorbisfile 

%.o:%.cpp
	g++ -I${HOME}/include/plib -I/net/ens/dbarthou/include -Wall -fPIC -fno-strict-aliasing -O2 -c enseirbot.cpp


drive.o: prolog.ll drive.ll
	cat $^ > .drive.ll
	llc -march=x86 -mcpu=i386 -filetype=obj -o $@ .drive.ll

clean: 
	-rm *.o *.so .drive.ll
