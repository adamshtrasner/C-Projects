#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define NUM_OF_ALPHABETS 26
#define NUM_OF_ARGS 5
#define USAGE_ERROR "Usage: cipher <encode|decode> <k> <source path file> <output path file>\n"
#define INVALID_VALUE_ERROR "The given shifts value is invalid\n"
#define INVALID_COMMAND_ERROR "The given command is invalid\n"
#define FILE_PATH_ERROR "The given file is invalid\n"
#define BUFF_LENGTH 100

/**
 * The Encode function: cycles every English alphabet
 * k letters to the right
 * @param s - a string
 * @param k - number of cycles
 * @param i - i'th letter of s
 */

void Encode(char s[], long int k, int i){
    int sub;
    while (s[i] != '\0'){
        if (s[i] >= 'a' && s[i] <= 'z'){
            /**
             * If we cycle the i'th letter k steps to the right
             * and it's larger than 'z', we cycle to 'z', change the letter
             * to 'a', and walk the remaining steps that are left from k
             */
            if (s[i] + k > 'z'){
                sub = 'z' - s[i];
                s[i] = (char)('a' + (k - 1 - sub));
            }
            else{
                s[i] = (char)(s[i] + k);
            }
        }
        if (s[i] >= 'A' && s[i] <= 'Z'){
            /**
             * Same explanation as with the non-capital letters above
             */
            if (s[i] + k > 'Z'){
                sub = 'Z' - s[i];
                s[i] = (char)('A' + (k - 1 - sub));
            }
            else{
                s[i] = (char)(s[i] + k);
            }
        }
        i++;
    }
}

/**
 * The Decode function: cycles every English alphabet
 * k letters to the left
 * @param s - a string
 * @param k - number of cycles
 * @param i - i'th letter of s
 */


void Decode(char s[], long int k, int i){
    int sub;
    while (s[i] != '\0'){
        if (s[i] >= 'a' && s[i] <= 'z'){
            /**
             * If we cycle the i'th letter k steps to the left
             * and it's smaller than 'a', we cycle to 'a', change the letter
             * to 'z', and walk the remaining steps that are left from k
             */
            if (s[i] - k < 'a'){
                sub = s[i] - 'a';
                s[i] = (char)('z' - (k - 1 - sub));
            }
            else{
                s[i] = (char)(s[i] - k);
            }
        }
        if (s[i] >= 'A' && s[i] <= 'Z'){
            /**
             * Same explanation as with the non-capital letters above
             */
            if (s[i] - k < 'A'){
                sub = s[i] - 'A';
                s[i] = (char)('Z' - (k - 1 - sub));
            }
            else{
                s[i] = (char)(s[i] - k);
            }
        }
        i++;
    }
}

int main(int argc, char *argv[]) {

    /**
     *  Number of arguments Error
     */
    if (argc != NUM_OF_ARGS) {
        fprintf(stderr, "%s", USAGE_ERROR);
        exit(EXIT_FAILURE);
    }

    /**
     * Number of cycles error - a negative number
     */
    long int k = strtol(argv[2], NULL, 10);
    if (k < 0) {
        fprintf(stderr, "%s", INVALID_VALUE_ERROR);
        exit(EXIT_FAILURE);
    }

    /**
     * Command Error
     */
    char *command = argv[1];
    if ((strcmp(command, "encode") != 0) && (strcmp(command, "decode") != 0)) {
        fprintf(stderr, "%s", INVALID_COMMAND_ERROR);
        exit(EXIT_FAILURE);
    }

    /**
     * File path error - non existent, unable to open, etc
     */
    FILE *read_file = fopen(argv[3], "r");
    FILE *write_file = fopen(argv[4], "w");
    if ((read_file == NULL) || (write_file == NULL)) {
        if (read_file != NULL){
            fclose(read_file);
        }
        if (write_file != NULL){
            fclose(write_file);
        }
        fprintf(stderr, "%s", FILE_PATH_ERROR);
        exit(EXIT_FAILURE);
    }

    /**
     * There are 26 letters in the english alphabet,
     * Therefore k is translated to k (mod 26) if it's
     * larger than 26
     */
    if (k >= NUM_OF_ALPHABETS){
        k = k % (NUM_OF_ALPHABETS);
    }

    char line[BUFF_LENGTH];

    /**
     * Comparing the command argument with "encode" and "decode
     * and execute the right function accordingly
     */

    if (!(strcmp(command, "encode"))) {
        while (fgets(line, sizeof(line), read_file) != NULL) {
            Encode(line, k, 0);
            fprintf(write_file, "%s", line);
        }
    }

    if (!(strcmp(command, "decode"))) {
        while (fgets(line, sizeof(line), read_file) != NULL) {
            Decode(line, k, 0);
            fprintf(write_file, "%s", line);
        }
    }

    fclose(read_file);
    fclose(write_file);
    return 0;
}
