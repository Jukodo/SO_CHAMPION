all: Player Referee Games clean

full: all run

fullReferee: Referee runReferee clean

fullPlayer: Player runPlayer clean

fullGames: Games runGames clean

Player: Player.o Utils.o
	gcc -o ../Execs/Player Player.o Utils.o

Player.o: ./Player/Player.c
	gcc -c -o Player.o ./Player/Player.c

Referee: Referee.o RService.o RThreads.o Utils.o
	gcc -o ../Execs/Referee Referee.o RService.o RThreads.o Utils.o -pthread

Referee.o: ./Referee/Referee.c
	gcc -c -o Referee.o ./Referee/Referee.c

RService.o: ./Referee/RService.c
	gcc -c -o RService.o ./Referee/RService.c

RThreads.o: ./Referee/RThreads.c
	gcc -c -o RThreads.o ./Referee/RThreads.c

Games: g_RNGGuess.o Utils.o
	gcc -o ../Execs/Games/g_RNGGuess g_RNGGuess.o Utils.o

g_RNGGuess.o: ./Games/g_RNGGuess.c
	gcc -c -o g_RNGGuess.o ./Games/g_RNGGuess.c

Utils.o: Utils/Utils.c
	gcc -c Utils/Utils.c

.PHONY: clean

clean:
	rm -f *.o

cleanObj:
	rm ../Execs/Referee ../Execs/Player ../Execs/Games/g_RNGGuess

runReferee:
	../Execs/Referee "D_300000" "W_60000"

runPlayer:
	../Execs/Player

runGames:
	../Execs/Games/g_RNGGuess

run:
	runReferee runPlayer runGames