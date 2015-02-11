/**
 * @file index.c
 * @author Michael Adam
 * @date April 2014
 *
 * This code implements the indexing of incoming data, and initiates a write to file when finished.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "index.h"

/* Variable declarations */
int mode;
char *docNo;
char * parseInt;
unsigned int docint;
tree wordtree;

/**
 * Sets up the variables needed to index.
 */
extern void begin_indexing(){
    printf("Indexing...\n");
    wordtree = NULL;
    docNo = malloc(sizeof(char) * 12);
}

/**
 * Performs post indexing operations (writing to disc and freeing memory).
 *
 */
extern void end_indexing(){
    printf("Indexing Complete\nWriting Index...");
    tree_write_to_file(wordtree);
    printf(" Done\n");
    
    wordtree = tree_free(wordtree);
    free(docNo);
}

/**
 * Confirms a start tag by setting the appropriate mode (or none)
 *
 * @param input The tag being opened.
 */
extern void start_tag(char const *input){
    if (strcmp(input, "docno") == 0){
        mode = 1;
    } else if (strcmp(input, "text") == 0 || strcmp(input, "in") == 0){
        mode = 2;
    } else {
        return;
    }
}

/**
 * Confirms the ending of a tag, and performs necessary followup operations
 * such as converting the document number to an integer.
 *
 * @param input The tag being closed.
 */
extern void end_tag(char const *input){
    if (strcmp(input, "docno") == 0){
        if (strlen(docNo) == 13){
            parseInt = malloc(sizeof(char)*9);
            memcpy(parseInt, &docNo[4], 9);
            parseInt[9] = '\0';
            docint = atoi(parseInt);
            
            free(parseInt);
        } else {
            printf("Uncrecognized DocNo Format: %s", docNo);
        }

    } else if (strcmp(input, "text") == 0){
        docNo[0] = '\0';
        
    } else {
        
    }
    
    mode = 0;
}

/**
 * Adds a word to the index if the appropriate mode is set.
 *
 * @param input The word being added to the index (maybe).
 */
extern void word(char const *input){
    if (strcmp(input, "the") != 0 && strcmp(input, "be") != 0 && strcmp(input, "to") != 0 && strcmp(input, "of") != 0 && strcmp(input, "and") != 0 && strcmp(input, "a") != 0 && strcmp(input, "in") != 0 && strcmp(input, "that") != 0){
        if (mode == 1) {
            if (strlen(docNo) == 0){
                strcpy(docNo, input);
                
            } else {
                strcat(docNo, input);
                
            }
        } else if (mode == 2) {
            wordtree = tree_insert(wordtree, input, docint);
        }
    }
    
}