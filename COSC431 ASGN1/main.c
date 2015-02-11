/**
 * @file main.c
 * @author Michael Adam
 * @date April 2014
 *
 * Uses command line parameters to set up and run the search engine / indexer.
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "search.h"
#include "parse.h"

int main(int argc, const char * argv[]){
    char *output = malloc(sizeof(char) * 20);
    char *searchTerms = NULL;
    int location = 0;
    int length = 0;
    int docno = 0;
    int occurrence = 0;
    int count = 0;
    size_t termSize;

    
    /* Indexer Mode
     * An input file is specified following -i on the commandline as "/path/to/file name", including quotation marks.
     */
    if (argv[1]){
        if (strcmp(argv[1], "-i") == 0){
            FILE *input = fopen(argv[2], "r");
            
            if (input == NULL) {
                printf("File not found\n");
                exit(EXIT_FAILURE);
            }
            
            parse(input);
            fclose(input);
          
        /* Print Mode
         * Dumps the contents of any index files contained in the application directory
         */
        } else if (strcmp(argv[1], "-p") == 0) {
            FILE *lookup = fopen("./lookup.bin", "r");
            FILE *postings = fopen("./postings.bin", "r");
            
            if (!lookup || !postings) {
                printf("Couldn't load index files");
            } else {
                printf("Printing index");
            }
            
            while (1){
                output[0] = '\0';
                location = 0;
                length = 0;
                docno = 0;
                occurrence = 0;
                
                if (fread(output, sizeof(char) * 20, 1, lookup) == 0) break;
                fread(&location, sizeof(int), 1, lookup);
                fread(&length, sizeof(int), 1, lookup);
                
                count ++;
    
                printf("Word: %s, \tPostings Location: %d, Postings Length: %d\n", output, location, length);
    
                for (int i = 0; i < length; i += sizeof(int) * 2){
                    fread(&docno, sizeof(int), 1, postings);
                    fread(&occurrence, sizeof(int), 1, postings);
                    printf("\tDoc Number: %d, Occurrence: %d\n", docno, occurrence);
                    docno = 0;
                    occurrence = 0;
                }
            }
            
            printf("Unique words: %d\n", count);
            
            fclose(lookup);
            fclose(postings);

        /* Search Mode (Custom)
         * Takes input line by line from stdin, using lookup and postings files as provided respectively
         * on the command line, formatted as -s "/path/to/lookup" "path/to/postings"
         */
        } else if (strcmp(argv[1], "-s") == 0) {
            FILE *lookup = fopen(argv[2], "rb");
            FILE *postings = fopen(argv[3], "rb");
            if (lookup == NULL || postings == NULL){
	            printf("Error getting index files");
	            exit(EXIT_FAILURE);
	        }
            
            while (getline(&searchTerms, &termSize, stdin)){
                if (searchTerms == NULL){
                    printf("Error getting input");
                    exit(EXIT_FAILURE);
                } else {
                    search(searchTerms, lookup, postings);
                }
            }
            
            free(searchTerms);
            fclose(lookup);
            fclose(postings);
        }
        
    /* Search Mode (Default)
     * Takes input line by line from stdin, using a lookup and postings file from the local directory.
     */
    } else {
        FILE *lookup = fopen("./lookup.bin", "rb");
        FILE *postings = fopen("./postings.bin", "rb");
        if (lookup == NULL || postings == NULL){
            printf("Error getting index files");
            exit(EXIT_FAILURE);
        }
        
        while (getline(&searchTerms, &termSize, stdin)){
            if (searchTerms == NULL){
                printf("Error getting input");
            } else {
                search(searchTerms, lookup, postings);
            }
        }
        
        free(searchTerms);
        fclose(lookup);
        fclose(postings);
    }
    
    free(output);
    
    return 0;

}

