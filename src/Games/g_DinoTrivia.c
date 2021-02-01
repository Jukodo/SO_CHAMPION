#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../Utils/Utils.h"

#define GAME_TITLE "DinoTrivia"

#define QUESTION 0
#define ANSWER 1
#define OPTIONS_OFFSET 2

int score = 0;
void handle_sigusr1(int sig) { exit(score); }

void Setup_SIGUSR1() {
  struct sigaction sa = {0};  //{0} - The struct starts with 0's
  sa.sa_flags = SA_RESTART;
  sa.sa_handler = &handle_sigusr1;
  sigaction(SIGUSR1, &sa, NULL);
}

char* triviaQuestions[52][6] = {
    {
        "What dinosaur name means \"fast thief?\"",
        "2",
        "PTERONODON",
        "VELOCIRAPTOR",
        "NANOTYRANNUS",
        "SINOCALLIOPTERYX",
    },
    {
        "What dinosaur fossil was originally mistaken for a type of bison?",
        "3",
        "STEGOSAURUS",
        "BUFFALOSAURUS",
        "TRICERATOPS",
        "ALLOSAURUS",
    },
    {
        "When did dinosaurs become extinct?",
        "3",
        "7 MILLION YEARS AGO",
        "285 MILLION YEARS AGO",
        "65 MILLION YEARS AGO",
        "2 BILLION YEARS AGO",
    },
    {
        "What were the direct ancestors of the dinosaurs?",
        "3",
        "EUKARYOTES",
        "SAPHEOSAURS",
        "ARCHOSAURS",
        "SILESAURS",
    },
    {
        "What was the lifespan of a T. rex?",
        "4",
        "10 TO 15 YEARS",
        "50 TO 60 YEARS",
        "100 TO 120 YEARS",
        "20 TO 30 YEARS",
    },
    {
        "The meteor that many scientists believe killed the dinosaurs struck "
        "in what modern-day country?",
        "4",
        "AUSTRALIA",
        "UNITED STATES",
        "SOUTH AFRICA",
        "MEXICO",
    },
    {
        "What dinosaur was, for many years, mistakenly called a Brontosaurus?",
        "1",
        "APATOSAURUS",
        "ARCANOSAURUS",
        "RIGIDOSAURUS",
        "BRACHIOSAURUS",
    },
    {
        "Which of the following dinosaurs had a giraffe-like neck?",
        "1",
        "BRACHIOSAURUS",
        "ANKYLOSAURUS",
        "ALLOSAURUS",
        "TORVOSAURUS",
    },
    {
        "What is the only dinosaur lineage to survive the mass extinction "
        "event?",
        "4",
        "LIZARDS",
        "FROGS",
        "SNAKES",
        "BIRDS",
    },
    {
        "Which dinosaur had fifteen horns?",
        "2",
        "STYRACOSAURUS",
        "KOSMOCERATOPS",
        "DIABLOCERATOPS",
        "PACHYRHINOSAURUS",
    },
    {
        "Which dinosaur had the smallest brain for its body size?",
        "1",
        "STEGOSAURUS",
        "T. REX",
        "TRICERATOPS",
        "BRACHIOSAURUS",
    },
    {
        "What fossil hunter theorized that bezoar stones were fossilized "
        "feces?",
        "2",
        "LUIS ALVAREZ",
        "MARY ANNING",
        "BARNUM BROWN",
        "WILLIAM BUCKLAND",
    },
    {
        "Which of the following dinosaurs was toothless?",
        "1",
        "OVIRAPTOR",
        "DIPLODOCUS",
        "COMPSOGNATHUS",
        "APATOSAURUS",
    },
    {
        "What would a sauropod use gastroliths for?",
        "2",
        "TO SHARPEN ITS CLAWS",
        "TO AID IN DIGESTION",
        "TO LURE ITS PREY",
        "TO ATTRACT A MATE",
    },
    {
        "What was the first mounted dinosaur skeleton?",
        "2",
        "T. REX",
        "HADROSAURUS",
        "STEGOSAURUS",
        "IGUANODON",
    },
    {
        "What was the wingspan of the smallest known pterodactyl?",
        "3",
        "10 CENTIMETERS",
        "10 METERS",
        "10 INCHES",
        "10 FEET",
    },
    {
        "Which dinosaur was named after a commercial airline?",
        "2",
        "DELTASAURUS",
        "QANTASSAURUS",
        "LUFTHANSASAURUS",
        "JETBLUEDON",
    },
    {
        "How many teeth did the Nigersaurus have?",
        "1",
        "500",
        "100",
        "300",
        "50",
    },
    {
        "What was the first dinosaur to be discovered?",
        "4",
        "TYRANNOSAURUS REX",
        "GIGANOTOSAURUS",
        "BRONTOSAURUS",
        "MEGALOSAURUS",
    },
    {
        "What dinosaur name means \"covered lizard?\"",
        "4",
        "DIMETRODON",
        "TRICERATOPS",
        "ANKYLOSAURUS",
        "STEGOSAURUS",
    },
    {
        "What dinosaur is believed to have weighed up to 110 tons?",
        "4",
        "BRAZILOSAURUS",
        "PERUSAURUS",
        "COLUMBIOSAURUS",
        "ARGENTINOSAURUS",
    },
    {
        "How long were the arms of an average Tyrannosaurus Rex?",
        "3",
        "2 METERS",
        "1/2 METER",
        "1 METER",
        "3 METERS",
    },
    {
        "When did dinosaurs first appear?",
        "3",
        "JURASSIC PERIOD",
        "CRETACEOUS PERIOD",
        "TRIASSIC PERIOD",
        "CAMBRIAN PERIOD",
    },
    {
        "How many claws did a velociraptor have on each hand?",
        "3",
        "2",
        "1",
        "3",
        "4",
    },
    {
        "What paleoartist produced a classic painting of Triceratops facing "
        "off against Tyrannosaurus rex?",
        "4",
        "ROBERT T. BAKKER",
        "GERHARD HEILMANN",
        "BENJAMIN WATERHOUSE HAWKINS",
        "CHARLES R. KNIGHT",
    },
    {
        "How much meat could Tyrannosaurus rex eat in one bite?",
        "3",
        "100 LBS",
        "250 LBS",
        "500 LBS",
        "1000 LBS",
    },
    {
        "Which of these dinosaurs was a meat eater?",
        "1",
        "DROMAEOSAURUS",
        "ANGULOMASTACATOR",
        "STEGOSAURUS",
        "TRICERATOPS",
    },
    {
        "What kind of dinosaur is the Toronto-based NBA basketball team named "
        "after?",
        "4",
        "TRICERATOPS",
        "T-REX",
        "PTERODACTYL",
        "RAPTOR",
    },
    {
        "What does \"dinosaur\"mean?",
        "3",
        "KILLER LIZARD",
        "EVIL LIZARD",
        "TERRIBLE LIZARD",
        "HUNGRY LIZARD",
    },
    {
        "What is the nickname of the most complete T-rex specimen ever "
        "discovered?",
        "3",
        "ANN",
        "MAY",
        "SUE",
        "EVE",
    },
    {
        "At the beginning of the Triassic period, how many continents were "
        "there?",
        "1",
        "1",
        "2",
        "3",
        "7",
    },
    {
        "Which extinction event wiped out the dinosaurs?",
        "2",
        "ORDOVICIAN-SILURIAN",
        "CRETACEOUS-PALEOGENE",
        "TRIASSIC-JURASSIC",
        "PERMIAN",
    },
    {
        "Which continents have dinosaur fossils been found on?",
        "2",
        "NORTH AMERICA",
        "ALL SEVEN CONTINENTS",
        "ASIA",
        "AFRICA",
    },
    {
        "What is the official state dinosaur of Colorado?",
        "2",
        "VELOCIRAPTOR",
        "STEGOSAURUS",
        "ALLOSAURUS",
        "TRICERATOPS",
    },
    {
        "Which of the following traits link birds to theropod dinosaurs?",
        "4",
        "AIR-FILLED BONES",
        "FEATHERS",
        "WISHBONE",
        "ALL OF THESE",
    },
    {
        "How are the two groups of dinosaurs differentiated?",
        "2",
        "METHOD OF REPRODUCTION",
        "HIP BONES",
        "EATING HABITS",
        "NUMBER OF CLAWS",
    },
    {
        "What was the first non-carnivorous theropod fossil to be discovered?",
        "3",
        "BARYONYX",
        "DEINONYCHUS",
        "THERIZINOSAUR",
        "TARBOSAURUS",
    },
    {
        "Where were the oldest dinosaur fossils in Canada found?",
        "4",
        "SASKATCHEWAN",
        "YUKON",
        "ALBERTA",
        "NOVA SCOTIA",
    },
    {
        "Who discovered the first fossilized dinosaur eggs?",
        "3",
        "GIDEON MANTELL",
        "ROY CHAPMAN ANDREWS",
        "JEAN-JACQUES POECH",
        "MARY ANNING",
    },
    {
        "How many fossils are necessary to identify a new dinosaur species?",
        "2",
        "7",
        "1",
        "5",
        "2",
    },
    {
        "In what country were dinosaur fossils thought to be dragon bones?",
        "3",
        "SOUTH AFRICA",
        "BRAZIL",
        "CHINA",
        "SAUDI ARABIA",
    },
    {
        "How many wings did a Microraptor have?",
        "1",
        "4",
        "6",
        "3",
        "2",
    },
    {
        "Which of the following dinosaurs was the smallest?",
        "2",
        "RAPTOR",
        "COMPSOGNATHUS",
        "TROODON",
        "DILOPHOSAURUS",
    },
    {
        "When the first Megalosaurus fossil was discovered, what was it "
        "mistaken for?",
        "3",
        "WHALE",
        "RHINOCEROS",
        "HUMAN GIANT",
        "BUFFALO",
    },
    {
        "The discovery of what dinosaur ushered in the age of the giant "
        "Sauropods?",
        "2",
        "APATOSAURUS",
        "DIPLODOCUS",
        "BRACHIOSAURUS",
        "SUPERSAURUS",
    },
    {
        "What was the name of the Tyrannosaurus in the TV series ",
        "3",
        "BIG ALICE",
        "DOPEY",
        "GRUMPY",
        "SPOT",
    },
    {
        "What kind of dinosaur is Fred Flintstone's pet Dino?",
        "1",
        "SNORKASAURUS",
        "YAMACERATOPS",
        "DODOSAURUS",
        "JABBERWOCKY",
    },
    {
        "What are the spikes on the end of a dinosaur's tail called?",
        "3",
        "KOTHUMUS",
        "SPINALIS CAPITIS",
        "THAGOMIZER",
        "CHEVRONS",
    },
    {
        "Which dinosaur name means \"hopping foot?\"",
        "2",
        "BAMBIRAPTOR ",
        "SALTOPUS",
        "CUTTYSARKUS",
        "YUTYRANNUS",
    },
    {
        "What was the first carnivorous dinosaur to be discovered in "
        "Antarctica?",
        "3",
        "GOJIRASAURUS",
        "COOKOSAURUS",
        "CRYOLOPHOSAURUS",
        "EREBUSAURUS",
    },
    {
        "Who outbid Leonardo DiCaprio in an auction for a dinosaur skull?",
        "4",
        "JOHN MALKOVICH",
        "JACK NICHOLSON",
        "JOAQUIN PHOENIX",
        "NICOLAS CAGE",
    },
    {
        "What was the first coelurosaur to be discovered and named from "
        "Africa?",
        "3",
        "MALAWISAURUS",
        "VULCANODON",
        "NQWEBASAURUS",
        "AARDONYX",
    },
};

void Print_Introduction() {
  char bufSTDOUT[STRING_LARGE];

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(bufSTDOUT, "\nHello! Welcome to the %s!\n", GAME_TITLE);
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(bufSTDOUT, "\tThe game flow is: \n");
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(bufSTDOUT,
          "\t - A random trivia question about dinosaurs will be picked\n");
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(bufSTDOUT, "\t - Four choices will be available for you to pick\n");
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(bufSTDOUT,
          "\t - You, the player, will have to pick the correct answer\n");
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(bufSTDOUT,
          "\t - Each correct answer will increase your score by 1\n");
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(
      bufSTDOUT,
      "\t - The question changes after you answer, either correct or not\n");
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(bufSTDOUT, "\t - To leave press CTRL-C or type #quit\n");
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));
}

int main(int argc, char** argv) {
  Utils_GenerateNewRandSeed();

  char bufSTDOUT[STRING_LARGE];
  char bufSTDIN[STRING_LARGE];

  Setup_SIGUSR1();

  Print_Introduction();

  bool flag_leave = true;

  while (true) {
    memset(bufSTDOUT, '\0', STRING_LARGE);
    sprintf(bufSTDOUT, "\n\tCurrent score: %d\n", score);
    write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

    int questionIndex = Utils_GetRandomNumber(0, 51);

    memset(bufSTDOUT, '\0', STRING_LARGE);
    sprintf(bufSTDOUT, "\t%s: \n", triviaQuestions[questionIndex][QUESTION]);
    write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

    // Print possibilities
    for (int i = OPTIONS_OFFSET; i < 4 + OPTIONS_OFFSET; i++) {
      memset(bufSTDOUT, '\0', STRING_LARGE);
      sprintf(bufSTDOUT, "\t%c) %s\n", (65 + i - OPTIONS_OFFSET),
              triviaQuestions[questionIndex][i]);
      write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));
    }

    memset(bufSTDOUT, '\0', STRING_LARGE);
    sprintf(bufSTDOUT, "\tAnswer: \n");
    write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

    memset(bufSTDIN, '\0', STRING_LARGE);
    read(STDIN_FILENO, bufSTDIN, STRING_LARGE);

    if (tolower(bufSTDIN[0]) ==
        tolower(atoi(triviaQuestions[questionIndex][ANSWER]) - 1 + 'A')) {
      score++;

      memset(bufSTDOUT, '\0', STRING_LARGE);
      sprintf(bufSTDOUT, "\tYou got it! +1 score! Keep going...\n");
      write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));
    } else {
      memset(bufSTDOUT, '\0', STRING_LARGE);
      sprintf(bufSTDOUT, "\tWrong...\n");
      write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));
    }
  }

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(bufSTDOUT, "\nThank you for playing %s!\n");
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

  memset(bufSTDOUT, '\0', STRING_LARGE);
  sprintf(bufSTDOUT, "\tYour final score: %d\n", score);
  write(STDOUT_FILENO, &bufSTDOUT, sizeof(bufSTDOUT));

  return false;
}