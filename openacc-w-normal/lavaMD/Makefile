# Example
# target: dependencies
	# command 1
	# command 2
          # .
          # .
          # .
	# command n

#C_C = pathcc
#C_C_FLAG = -acc
C_C = pgc++
C_C_FLAG =-Mllvm -w -mp -acc -ta=tesla -Minfo=accel -O3 -std=c++11 -Mcuda

# C_C_FLAG += -g -O0

C_C_FLAG += -I$(GECKO_HOME)/ -L$(GECKO_HOME)/lib
C_C_LIB = -lgecko

# link objects (binaries) together
a.out:		main.o \
			./kernel/kernel_acc.o \
			./util/num/num.o \
			./util/timer/timer.o
	$(C_C) $(C_C_FLAG) main.o \
			./kernel/kernel_acc.o \
			./util/num/num.o \
			./util/timer/timer.o \
			-lm \
			-o lavaMD $(C_C_LIB)
			
# compile function files into objects (binaries)
main.o:		main.h \
			main.c \
			./kernel/kernel_acc.h \
			./kernel/kernel_acc.c \
			./util/num/num.h \
			./util/num/num.c \
			./util/timer/timer.h \
			./util/timer/timer.c
	$(C_C) $(C_C_FLAG) main.c \
			-c \
			-o main.o 

./kernel/kernel_acc.o:	./kernel/kernel_acc.h \
						./kernel/kernel_acc.c
	$(C_C) $(C_C_FLAG) 	./kernel/kernel_acc.c \
						-c \
						-o ./kernel/kernel_acc.o 

./util/num/num.o:	./util/num/num.h \
					./util/num/num.c
	$(C_C) $(C_C_FLAG) ./util/num/num.c \
					-c \
					-o ./util/num/num.o 

./util/timer/timer.o:	./util/timer/timer.h \
						./util/timer/timer.c
	$(C_C) $(C_C_FLAG) 	./util/timer/timer.c \
						-c \
						-o ./util/timer/timer.o 
						

# delete all object and executable files
clean:
	rm -rf	\
		*.o \
		./kernel/*.o \
		./util/num/*.o \
		./util/timer/*.o \
		lavaMD
