#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "game_initialization.h"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

char* initialize_player_word(unsigned short* nbrOfChar)
{
    char* playerWord;
    *nbrOfChar = 0;
    while (*nbrOfChar > 12 || *nbrOfChar < 6) {
        fprintf(stdout, "Pensez a un mot entre 6 et 12 lettres,\n"
                        "puis donnez le nombre de lettres qu'il contient : ");
        fscanf(stdin, "%hd", nbrOfChar);
    }
    playerWord = (char *) calloc(*nbrOfChar, sizeof(char));
    for (int i = 0; i < *nbrOfChar; ++i) {
        playerWord[i] = '*';
    }
    return playerWord;
}

const char* get_user_word_choice(char * word)
{
    short wordLength;
    do {
        fprintf(stdout, "\nChoisissez un mot entre 6 et 12 lettres : ");
        fscanf(stdin, "%s", word);
        wordLength = strlen(word);
        printf("%d ; %s", wordLength, word);
        while(getchar() != '\n');
    } while (wordLength < 6 || wordLength > 12);
    return word;
}
#pragma clang diagnostic pop
