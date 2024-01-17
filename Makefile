CC=g++
INC_DIR = 
LIB_DIR = 
CFLAGS:= $(CFLAGS) -O2 -g -Wall -std=c++11
DEPS =  
OBJ = fbplayer.o Eva.o image.o 
LDFLAGS = -lm -lpthread
PROGRAMS = fbplayer

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) -I$(INC_DIR)
fbplayer: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -I$(INC_DIR) -L$(LIB_DIR) $(LDFLAGS)
	

all: fbplayer

clean:
	rm -f *.o *~ fbplayer 

