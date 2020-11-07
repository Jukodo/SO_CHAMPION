all: Client Referee Games clean

Client: 

Referee:

Games: g_RNGGuess.o utils.o
	gcc -o g_RNGGuess g_RNGGuess.o utils.o

g_RNGGuess.o: g_RNGGuess.c
	gcc -c -o g_RNGGuess.o g_RNGGuess.c

Referee: Referee.o
	gcc -o Referee Referee.o

Referee.o: Referee.c
	gcc -c -o Referee.o Referee.c

utils.o: utils/utils.c
	gcc -c utils/utils.c

.PHONY: clean

clean:
	rm -f *.o Games Referee

run:
	./Referee "TEST_PARAM"
	./g_RNGGuess