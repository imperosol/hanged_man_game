#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

/**
 * @file main.c
 * @brief Programme qui permet à un joueur de joueur au pendu contre l'ordinateur.
 * C'est l'ordinateur qui doit deviner le mot.
 */

FILE* open_file(const char* fileName, const char* mode);
void get_most_used_char_in_file(FILE* file, unsigned short* alphabetArray);
char get_most_used_char_in_array(const unsigned short* alphabetArray);
void copy_same_length_words(FILE* source, FILE* dest, const unsigned short nbrOfChar);
void display_word(const char* word, const unsigned short wordLength);
short is_to_be_kept(const char* word, const char toKeep, const unsigned short position);
short is_to_be_deleted(const char* word, const char toDelete, const unsigned short wordLength);
FILE* delete_impossible_word(FILE* file, const char toDelete, const unsigned short position);
FILE* delete_words_with_letter(FILE* file, const char toDelete, const unsigned short wordLength);
bool is_word_completed(const char* word, const unsigned short wordLength);
bool ask_player_char_in_word(const char letter);
bool is_file_empty(FILE* file);
int* discover_letter(char* word, const char letter, unsigned short* arraySize);
char* initialize_player_word(unsigned short* nbrOfChar);

int main(void)
{
    /*Variables initialization*/
    bool isGameFinished = false;
    bool already_used_char[26] = {false}; // List of already used char
    int* position = NULL; // List of the positions of the letters in the word
    unsigned short nbrOfPositions;
    unsigned short remainingTry = 7; // Remaining possible wrong guesses before defeat
    unsigned short nbrOfChar; // Nbr of characters in the word
    char* playerWord = NULL;
    unsigned short most_used_char[26]; // List of the most used characters in the database
    char charToGuess;

    /*Create the player word*/
    playerWord = initialize_player_word(&nbrOfChar);

    /* Create a temporary file which will be deleted at the end of the program */
    FILE * const_dictionary = open_file("dictionary.txt", "r");
    FILE * game_dictionary = open_file("temp_dictionary.txt", "w+");
    copy_same_length_words(const_dictionary, game_dictionary, nbrOfChar+1);
    fclose(const_dictionary);
    while (!isGameFinished){
        display_word(playerWord, nbrOfChar);
        fprintf(stdout, "Coups restants : %d\n", remainingTry);
        if (!remainingTry || is_word_completed(playerWord, nbrOfChar)) {
            /* The computer has used all of its possibles wrong guesses
             * Or he has won by completing the word */
            isGameFinished = true;
        } else if (is_file_empty(game_dictionary)){
            /* If the database contains no more word */
            puts("Je ne sais pas de quel mot il s'agit");
            remainingTry = 0;
            isGameFinished = true;
        } else {
            get_most_used_char_in_file(game_dictionary, most_used_char);
            /* Remove the already used characters from the most_used_char array */
            for (int i = 0; i < 26; ++i) {
                if (already_used_char[i]) {
                    most_used_char[i] = 0;
                }
            }
            charToGuess = get_most_used_char_in_array(most_used_char);

            /* Ask to the player if the current most used letter is in the player word
             * and act consequently */
            if (ask_player_char_in_word(charToGuess)) {
                /*The computer asked a letter which is in the player word*/
                position = discover_letter(playerWord, charToGuess, &nbrOfPositions);
                for (int i = 0; i < nbrOfPositions; ++i) {
                    game_dictionary = delete_impossible_word(game_dictionary, charToGuess, position[i]);
                }
                free(position);
            } else {
                /*The computer asked a wrong letter*/
                remainingTry--;
                game_dictionary = delete_words_with_letter(game_dictionary, charToGuess, nbrOfChar);
            }
            already_used_char[charToGuess - 'a'] = true;
        }
    }
    fclose(game_dictionary);
    if (remainingTry) {
        /*Computer victory*/
        fprintf(stdout, "Victoire pour moi.\nVotre mot est : %s", playerWord);
    } else {
        /*Human player victory*/
        puts("J'ai perdu. Quel était votre mot ?");
        fscanf(stdin, "%s", playerWord);
        const_dictionary = open_file("dictionary.txt", "a");
        fprintf(const_dictionary, "\n%s", playerWord);
        fclose(const_dictionary);
    }
    /*Buffer Clear*/
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
    puts("Appuyer sur enter pour quitter");
    while (getchar() != '\n');
    return 0;
}

void get_most_used_char_in_file(FILE* file, unsigned short* alphabetArray)
{
    for (int i = 0; i < 26; ++i) {
        alphabetArray[i] = 0;
    }
    char temp;
    rewind(file);
    while (!feof(file)) {
        temp = fgetc(file);
        if (isalpha(temp)) {
            alphabetArray[temp - 'a']++;
        }
    }
}

char get_most_used_char_in_array(const unsigned short* alphabetArray)
{
    unsigned short max = 0;
    for (int i = 0; i < 26; ++i) {
        if (alphabetArray[i] > alphabetArray[max]) {
            max = i;
        }
    }
    return 'a' + max;
}

FILE* open_file(const char* fileName, const char* mode)
{
    errno_t err;
    FILE * toOpen = NULL;
    if ((err = fopen_s(&toOpen, fileName, mode))) {
        fprintf(stderr, "cannot open file '%s': %s\n", fileName, strerror(err));
        exit(1);
    } else {
        return toOpen;
    }
}

void copy_same_length_words(FILE* source, FILE* dest, const unsigned short nbrOfChar)
{
    char word[20];
    rewind(source);
    rewind(dest);
    while (!feof(source)) {
        fgets(word, 20, source);
        if (strlen(word) == nbrOfChar) {
            fprintf(dest, "%s", word);
        }
    }
    return;
}

void display_word(const char* word, const unsigned short wordLength)
{
    fprintf(stdout, "%s\n", word);
    for (int i = 1; i <= wordLength; ++i) {
        fprintf(stdout, "%d", i);
    }
    fprintf(stdout, "\n");
}

FILE* delete_words_with_letter(FILE* file, const char toDelete, const unsigned short wordLength)
{
    char word[20];
    FILE* newFile = NULL;
    newFile = open_file("temp.txt", "w");
    rewind(file);
    while (!feof(file)) {
        fgets(word, 20, file);
        if (!is_to_be_deleted(word, toDelete, wordLength) && !feof(file)) {
            fprintf(newFile, "%s", word);
        }
    }
    fclose(file);
    fclose(newFile);
    remove("temp_dictionary.txt");
    rename("temp.txt", "temp_dictionary.txt");
    newFile = open_file("temp_dictionary.txt", "r");
    return newFile;
}

FILE* delete_impossible_word(FILE* file, const char toDelete, const unsigned short position)
{
    char word[20];
    FILE* newFile = NULL;
    newFile = open_file("temp.txt", "w");
    rewind(file);
    while (!feof(file)) {
        fgets(word, 20, file);
        if (is_to_be_kept(word, toDelete, position) && !feof(file)) {
            fprintf(newFile, "%s", word);
        }
    }
    fclose(file);
    fclose(newFile);
    remove("temp_dictionary.txt");
    rename("temp.txt", "temp_dictionary.txt");
    newFile = open_file("temp_dictionary.txt", "r");
    return newFile;
}

short is_to_be_kept(const char* word, const char toKeep, const unsigned short position)
{
    if (word[position] == toKeep) {
        return 1;
    } else {
        return 0;
    }
}

short is_to_be_deleted(const char* word, const char toDelete, const unsigned short wordLength)
{
    for (int i = 0; i < wordLength; ++i) {
        if (word[i] == toDelete) {
            return 1;
        }
    }
    return 0;
}

bool is_word_completed(const char* word, const unsigned short wordLength)
{
    for (int i = 0; i < wordLength; ++i) {
        if (word[i] == '*') {
            return false;
        }
    }
    return true;
}

bool ask_player_char_in_word(const char letter)
{
    /*Begin by clearing the buffer*/
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
    char choice = 'a';
    while (choice != 'o' && choice != 'n') {
        fprintf(stdout, "Est-ce que la lettre %c est dans le mot (o/n) ? ", letter);
        fscanf(stdin, "%c", &choice);
    }
    return (choice == 'o');
}

bool is_file_empty(FILE* file)
{
    fseek(file, 0, SEEK_END);
    return (!ftell(file));
}

int* discover_letter(char* word, const char letter, unsigned short* arraySize)
{
    /*Begin by clearing the buffer*/
    char c;
    while ((c = getchar()) != '\n' && c != EOF);

    bool is_valid_choice;
    int* position = NULL;
    fprintf(stdout, "Quel est le nombre d'apparitions de cette lettre ? ");
    fscanf(stdin, "%hd", arraySize);
    position = calloc(*arraySize, sizeof (int));
    for (int i = 0; i < *arraySize; ++i) {
        is_valid_choice = false;
        while (!is_valid_choice) {
            fprintf(stdout, "Quelle est la position %d de la lettre %c ? ", i + 1, letter);
            fscanf(stdin, "%d", &position[i]);
            (position[i])--;
            if (word[position[i]] != '*') {
                puts("Invalide, il y a deja une lettre ici");
            } else {
                is_valid_choice = true;
                word[position[i]] = letter;
            }
        }
    }
    return position;
}

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

#pragma clang diagnostic pop
