all: Player Referee Games clean

full: all run

Player: Player.o utils.o
	gcc -o Player Player.o utils.o

Player.o: Player.c
	gcc -c -o Player.o Player.c

Referee: Referee.o utils.o
	gcc -o Referee Referee.o utils.o

Referee.o: Referee.c
	gcc -c -o Referee.o Referee.c

Games: g_RNGGuess.o utils.o
	gcc -o g_RNGGuess g_RNGGuess.o utils.o

g_RNGGuess.o: g_RNGGuess.c
	gcc -c -o g_RNGGuess.o g_RNGGuess.c

utils.o: utils/utils.c
	gcc -c utils/utils.c

.PHONY: clean

clean:
	rm -f *.o

cleanObj:
	rm Referee Player g_RNGGuess

run:
	./Referee "D_300000" "W_60000"
	./Player
	./g_RNGGuess