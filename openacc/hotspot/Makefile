
GECKO_HOME?=$(HOME)/gecko

# C compiler
CC = pgc++
CC_FLAGS = -acc -ta=tesla -Minfo=accel -O3 -std=c++11 -Mcuda

# CC_FLAGS += -g -O0

hotspot: hotspot_openacc.cpp
	$(CC) $(CC_FLAGS) -I$(GECKO_HOME)/ -L$(GECKO_HOME)/lib hotspot_openacc.cpp -o hotspot -lgecko

clean:
	rm -f hotspot
