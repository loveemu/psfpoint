CC       = gcc
CXX      = g++
CFLAGS   = -O2 -fomit-frame-pointer -Wall
CXXFLAGS = -O2 -fomit-frame-pointer -Wall
LD       = g++
LFLAGS   = -s
OBJECTS  = psfpoint.o psftag.o
PROGRAM  = psfpoint

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(LD) $(LFLAGS) -o $@ $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<
