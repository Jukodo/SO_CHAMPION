all: Player Referee Games clean

full: all run

fullReferee: Referee clean runReferee

fullPlayer: Player clean runPlayer

fullGames: Games clean runGames

Player: Player.o PService.o Utils.o
	gcc -o ../Execs/Player ../tmp/Player.o ../tmp/PService.o ../tmp/Utils.o

Player.o: ./Player/Player.c
	gcc -c -o ../tmp/Player.o ./Player/Player.c

PService.o: ./Player/PService.c
	gcc -c -o ../tmp/PService.o ./Player/PService.c

Referee: Referee.o RService.o RThreads.o Utils.o
	gcc -o ../Execs/Referee ../tmp/Referee.o ../tmp/RService.o ../tmp/RThreads.o ../tmp/Utils.o -pthread

Referee.o: ./Referee/Referee.c
	gcc -c -o ../tmp/Referee.o ./Referee/Referee.c

RService.o: ./Referee/RService.c
	gcc -c -o ../tmp/RService.o ./Referee/RService.c

RThreads.o: ./Referee/RThreads.c
	gcc -c -o ../tmp/RThreads.o ./Referee/RThreads.c

Games: g_RNGGuess.o Utils.o
	gcc -o ../Execs/Games/g_RNGGuess ../tmp/g_RNGGuess.o ../tmp/Utils.o

g_RNGGuess.o: ./Games/g_RNGGuess.c
	gcc -c -o ../tmp/g_RNGGuess.o ./Games/g_RNGGuess.c

Utils.o: Utils/Utils.c
	gcc -c -o ../tmp/Utils.o ./Utils/Utils.c

.PHONY: clean

clean:
	rm -f ../tmp/*.o

cleanObj:
	rm ../Execs/Referee ../Execs/Player ../Execs/Games/g_RNGGuess

run: runReferee runPlayer runGames

runReferee:
	../Execs/Referee "D_300000" "W_60000"

runPlayer:
	../Execs/Player

runGames:
	../Execs/Games/g_RNGGuess