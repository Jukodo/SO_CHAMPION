all: Player Referee Games clean

full: all run

Player: Player.o utils.o
	gcc -o ../Execs/Player Player.o utils.o

Player.o: Player.c
	gcc -c -o Player.o Player.c

Referee: Referee.o RService.o RThreads.o utils.o
	gcc -o ../Execs/Referee Referee.o RService.o RThreads.o utils.o -pthread

Referee.o: Referee.c
	gcc -c -o Referee.o Referee.c

RService.o: RService.c
	gcc -c -o RService.o RService.c

RThreads.o: RThreads.c
	gcc -c -o RThreads.o RThreads.c

Games: g_RNGGuess.o utils.o
	gcc -o ../Execs/Games/g_RNGGuess g_RNGGuess.o utils.o

g_RNGGuess.o: g_RNGGuess.c
	gcc -c -o g_RNGGuess.o g_RNGGuess.c

utils.o: utils/utils.c
	gcc -c utils/utils.c

.PHONY: clean

clean:
	rm -f *.o

cleanObj:
	rm ../Execs/Referee ../Execs/Player ../Execs/Games/g_RNGGuess

run:
	../Execs/Referee "D_300000" "W_60000"
	../Execs/Player
	../Execs/Games/g_RNGGuess