CC     = gcc
CFLAGS = -O2 -fomit-frame-pointer -Wall
LD     = gcc
LFLAGS = -s
OBJECTS = psfpoint.o psftag.o
PROGRAM = psfpoint

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(LD) $(LFLAGS) -o $@ $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<
