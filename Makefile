# Various flags
CXX  = mpic++
LINK = $(CXX)
#CXXFLAGS = -I -Wall -g 
CXXFLAGS = -g -Wall -O3 -fopenmp --std=c++11 #-I #-Wall -O3 -funroll-loops -pipe 
#CXXFLAGS = -g -Wall -fopenmp #-I #-Wall -O3 -funroll-loops -pipe 
LFLAGS =  -g -fopenmp -Wall -O3


TARGET  = ompInfomap


HEADER  = Node.h Module.h FileIO.h timing.h global.h
FILES = OmpRelaxmap.cpp Node.cpp Module.cpp FileIO.cpp timing.cpp global.cpp


OBJECTS = $(FILES:.cpp=.o)

ASMS = $(FILES:.cpp=.s)


$(TARGET): ${OBJECTS}
	$(LINK) $(LFLAGS) $^ -o $@ -lmetis

%.s:%.cpp
	$(LINK) $(LFLAGS) --std=c++11 -fverbose-asm -S $< -o $@


asm: $(ASMS)



all: $(TARGET) asm

clean:
	rm -f $(OBJECTS)

distclean:
	rm -f $(OBJECTS) $(TARGET)

# Compile and dependency
$(OBJECTS): $(HEADER) Makefile




