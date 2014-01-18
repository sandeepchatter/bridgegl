CC = g++

CFLAGS  = -Wall -g -w
LDFLAGS = -lm

ifeq ($(GV),GTK+3)
	CFLAGS  += `pkg-config gtk+-3.0 --cflags`
	LDFLAGS += `pkg-config gtk+-3.0 --libs`
else
	CFLAGS  += `pkg-config gtk+-2.0 --cflags`
	LDFLAGS += `pkg-config gtk+-2.0 --libs`
endif
#CFLAGS += "-DGSEAL_ENABLE" -- dont use, internal bug
CFLAGS += "-DGTK_DISABLE_SINGLE_INCLUDES"
CFLAGS += "-DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED"

# the name of the executable
EXE = application

# the object files generated from cpp files
OBJ = application.o bgl_graphics.o

# the source files
SRC = application.cpp bgl_graphics.cpp

#the header files
H = bgl_graphics.h

$(EXE): $(OBJ)
	@echo "========== Used $(GV) to build executable: $(EXE) ==========="
	$(CC) $(CFLAGS) $(OBJ) -o $(EXE) $(LDFLAGS)

application.o: application.cpp $(H)
	$(CC) -c  $(CFLAGS) $(LDFLAGS) application.cpp

bgl_graphics.o: bgl_graphics.cpp $(H)
ifeq ($(GV),GTK+3)
	$(CC) -c -D $(GV) $(CFLAGS) $(LDFLAGS) bgl_graphics.cpp
else
	$(CC) -c $(CFLAGS) $(LDFLAGS) bgl_graphics.cpp
endif	
clean:
	rm -f ap *.o
	
	
