all: Player Referee Games clean

full: all run

fullReferee: Referee Games clean runReferee

fullPlayer: Player clean runPlayer

fullGames: Games clean runGames

# gameArith: Utils.o
# 	gcc -c -o ../tmp/g_Arithmetic.o ./Games/g_Arithmetic.c
# 	gcc -o ../Execs/Games/g_Arithmetic ../tmp/g_Arithmetic.o ../tmp/Utils.o -lm
# 	../Execs/Games/g_Arithmetic

# gameDinoTrivia: Utils.o
# 	gcc -c -o ../tmp/g_DinoTrivia.o ./Games/g_DinoTrivia.c
# 	gcc -o ../Execs/Games/g_DinoTrivia ../tmp/g_DinoTrivia.o ../tmp/Utils.o -lm
# 	../Execs/Games/g_DinoTrivia

# gameTranslation: Utils.o
# 	gcc -c -o ../tmp/g_Translation.o ./Games/g_Translation.c
# 	gcc -o ../Execs/Games/g_Translation ../tmp/g_Translation.o ../tmp/Utils.o -lm
# 	../Execs/Games/g_Translation

Player: Player.o PService.o PThreads.o Utils.o
	gcc -o ../Execs/Player ../tmp/Player.o ../tmp/PService.o ../tmp/PThreads.o ../tmp/Utils.o -pthread -lm

Player.o: ./Player/Player.c
	gcc -c -o ../tmp/Player.o ./Player/Player.c

PService.o: ./Player/PService.c
	gcc -c -o ../tmp/PService.o ./Player/PService.c

PThreads.o: ./Player/PThreads.c
	gcc -c -o ../tmp/PThreads.o ./Player/PThreads.c

Referee: Referee.o RService.o RThreads.o Utils.o
	gcc -o ../Execs/Referee ../tmp/Referee.o ../tmp/RService.o ../tmp/RThreads.o ../tmp/Utils.o -pthread -lm

Referee.o: ./Referee/Referee.c
	gcc -c -o ../tmp/Referee.o ./Referee/Referee.c

RService.o: ./Referee/RService.c
	gcc -c -o ../tmp/RService.o ./Referee/RService.c

RThreads.o: ./Referee/RThreads.c
	gcc -c -o ../tmp/RThreads.o ./Referee/RThreads.c

Games: g_RNGGuess.o g_Arithmetic.o g_DinoTrivia.o g_Translation.o Utils.o
	gcc -o ../Execs/Games/g_RNGGuess ../tmp/g_RNGGuess.o ../tmp/Utils.o -lm
	gcc -o ../Execs/Games/g_Arithmetic ../tmp/g_Arithmetic.o ../tmp/Utils.o -lm
	gcc -o ../Execs/Games/g_DinoTrivia ../tmp/g_DinoTrivia.o ../tmp/Utils.o -lm
	gcc -o ../Execs/Games/g_Translation ../tmp/g_Translation.o ../tmp/Utils.o -lm

g_RNGGuess.o: ./Games/g_RNGGuess.c
	gcc -c -o ../tmp/g_RNGGuess.o ./Games/g_RNGGuess.c

g_Arithmetic.o: ./Games/g_Arithmetic.c
	gcc -c -o ../tmp/g_Arithmetic.o ./Games/g_Arithmetic.c

g_DinoTrivia.o: ./Games/g_DinoTrivia.c
	gcc -c -o ../tmp/g_DinoTrivia.o ./Games/g_DinoTrivia.c

g_Translation.o: ./Games/g_Translation.c
	gcc -c -o ../tmp/g_Translation.o ./Games/g_Translation.c

Utils.o: Utils/Utils.c
	gcc -c -o ../tmp/Utils.o ./Utils/Utils.c 

.PHONY: clean

clean:
	rm -f ../tmp/*.o

cleanObj:
	rm ../Execs/Referee ../Execs/Player ../Execs/Games/g_RNGGuess ../Execs/Games/g_Arithmetic ../Execs/Games/g_DinoTrivia ../Execs/Games/g_Translation

run: runReferee runPlayer runGames

runReferee:
	../Execs/Referee "W_60" "D_300"

runPlayer:
	../Execs/Player

runGames:
	../Execs/Games/g_RNGGuess
	../Execs/Games/g_Arithmetic
	../Execs/Games/g_DinoTrivia
	../Execs/Games/g_Translation