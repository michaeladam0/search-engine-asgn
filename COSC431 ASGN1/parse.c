/**
 * @file tree.c
 * @author Michael Adam
 * @date April
 *
 * This code takes characters from a filestream and parses them to extract the individual words and relevant metadata from the file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parse.h"

/* Variable declarations */
char *newWord;
size_t wordIndex;
int wordIsTag;
int wordIsEndTag;
FILE inputStream;

/**
 * Acquires a character from the current read point of the open file and returns it.
 *
 * @return A new character from the input file.
 */
int get(void){
    return fgetc(&inputStream);
}

/**
 * Continuously reads in characters from a given file till the end is reached,
 * sending characters to be indexed as appropriate and skipping unwanted characters
 * and markup.
 *
 * @param stream The file being read.
 */
void parse(FILE *stream){
    inputStream = *stream;
    int c = get();
    
    wordIndex = 0;
    newWord = malloc(sizeof(char)*100);
    wordIsEndTag = 0;
    wordIsTag = 0;
    
    begin_indexing();
    
    while (c != EOF) {
        if (isalnum(c)) {
            add_to_word(c);
            c = get();
            
        } else if (c == '<') {
            end_word();
            c = get();
            wordIsTag = 1;
            
            if (c == '/') {
                wordIsEndTag = 1;
                c = get();
            }
                
        } else if (c=='&') {
            while (c!=';') c = get();
            c = get();
            
        } else if (c == '\'') {
            c = get();
            if (c == 's') {
                c = get();
            }
            
        }else if (c == ' ' || c == '>' || c == '-') {
            end_word();
            c = get();

        } else if (c == '\n') {
            end_word();
            c = get();
            
        } else {
            c = get();
        }
    }
    
    end_indexing();
    free(newWord);
}

/**
 * Signals the end of a word by closing the string and sending it to be indexed as a word,
 * tag or end tag as appropriate.
 *
 */
void end_word() {
    newWord[wordIndex] = '\0';
    wordIndex = 0;
    
    if (wordIsTag == 1){
        if (wordIsEndTag == 1){
            end_tag(newWord);
            wordIsEndTag = 0;
            
        } else {
            start_tag(newWord);
        }
        
        wordIsTag = 0;
        
    } else if (strlen(newWord) > 1){
        word(newWord);
    }
}

/**
 * Appends a letter to the current word.
 *
 * @param c The character being appended.
 */
void add_to_word(char c) {
    char newC = tolower(c);
    newWord[wordIndex++] = newC;
}