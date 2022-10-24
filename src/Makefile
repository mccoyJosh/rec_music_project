SHELL='bash'

CC = g++
MPCC = mpic++
CURL =
CFLAGS = -O3
OPENMP = #-fopenmp
LIBS = -lcurl


TARGETS = Run

all:	$(TARGETS)

Run: Main.o SongManager.o FileManager.o
	$(CC) -o $@ $(LIBS) $(OPENMP) Main.o SongManager.o FileManager.o -l curl


Main.o: Main.cpp
	$(CC) -c $(CFLAGS) Main.cpp
FileManager.o: FileManager.cpp
	$(CC) -c $(CFLAGS) FileManager.cpp
SongManager.o: SongManager.cpp
	$(CC) -c $(CURL) $(OPENMP) $(CFLAGS) SongManager.cpp


clean:
	rm -rf SongManager.o && rm -rf Main.o && rm -rf FileManager.o && rm -rf curl
