
LOCAL_CC = gcc -g -O3 -Wall
CC = pgc++
CFLAGS = -w -mp -acc -ta=tesla,multicore -Minfo=accel -O3 -std=c++11 -Mcuda

CFLAGS += -I$(GECKO_HOME)/ -L$(GECKO_HOME)/lib
CLIB = -lgecko



all : nn

clean :
	rm -rf *.o nn hurricane_gen

nn : nn_openacc.cpp
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -lm $(CLIB)

hurricane_gen : hurricane_gen.c
	$(LOCAL_CC) -o $@ $< -lm

