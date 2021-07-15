# Various flags
CXX  = mpic++
LINK = $(CXX)
#CXXFLAGS = -I -Wall -g 
CXXFLAGS = -g -Wall -O3 -fopenmp --std=c++11  -I/usr/asa_install/include #-I #-Wall -O3 -funroll-loops -pipe 
#CXXFLAGS = -g -Wall -fopenmp #-I #-Wall -O3 -funroll-loops -pipe 
LFLAGS =  -g -fopenmp -Wall -Werror -O3

LIB=-L/usr/asa_install/lib/x86_64-linux-gnu
LDFLAGS="-Wl,-rpath,/usr/asa_install/lib/x86_64-linux-gnu"

TARGET  = ompInfomap

HEADER  = Node.h Module.h FileIO.h timing.h global.h
FILES = OmpRelaxmap.cpp Node.cpp Module.cpp FileIO.cpp timing.cpp global.cpp

OBJECTS = $(FILES:.cpp=.o)

$(TARGET): ${OBJECTS}
	$(LINK) $(LFLAGS) $(LIB) $(LDFLAGS) $^ -o  $@ -lasa

all: $(TARGET)

clean:
	rm -f $(OBJECTS)

distclean:
	rm -f $(OBJECTS) $(TARGET)

# Compile and dependency
$(OBJECTS): $(HEADER) Makefile




