CC = g++

CFLAGS  = -Wall -g -w
LDFLAGS = -lm

CFLAGS  += `pkg-config gtk+-2.0 --cflags`
LDFLAGS += `pkg-config gtk+-2.0 --libs`

#CFLAGS += "-DGSEAL_ENABLE"

# the name of the executable
EXE = application

# the object files generated from cpp files
OBJ = application.o bgl_graphics.o

# the source files
SRC = application.cpp bgl_graphics.cpp

#the header files
H = bgl_graphics.h


$(EXE): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) -o $(EXE)

application.o: application.cpp $(H)
	$(CC) -c  $(CFLAGS) $(LDFLAGS) application.cpp

bgl_graphics.o: bgl_graphics.cpp $(H)
	$(CC) -c  $(CFLAGS) $(LDFLAGS) bgl_graphics.cpp 
	
clean:
	rm -f ap *.o
