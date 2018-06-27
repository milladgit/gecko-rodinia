#makefile

CC = pgc++
FLAGS = -w -mp -acc -ta=tesla,multicore -Minfo=accel -O3 -Mcuda -std=c++11 

FLAGS += -I$(GECKO_HOME)/ -L$(GECKO_HOME)/lib
CC_LIB = -lgecko


all: ex_particle_OPENACC_seq.c
	$(CC) $(FLAGS) ex_particle_OPENACC_seq.c -o particle_filter -lm $(CC_LIB)

# openmp: ex_particle_OPENACC_seq.c
# 	$(CC) -O3 $(FLAGS) ex_particle_OPENACC_seq.c -o particle_filter -lm
	

clean:
	rm -rf *.o particle_filter
