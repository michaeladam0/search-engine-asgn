/**
 * @file search.c
 * @author Michael Adam
 * @date April 2014
 *
 * This code accesses the index and looks for search terms, returning document numbers and relevance scores.
 * Binary search of the index was not completed in time, so a linear search has been left in to demonstrate 
 * functionality, and two attempts at producing functioning binary search code have been left in the comments
 * below.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "search.h"

/* Variable declarations */
resultstree resultsFirstPass;
resultstree resultsSecondPass;
FILE *lookup;
FILE *postings;


/* Struct definitions */
struct results_tree_node {
    int key;
    float rsv;
    
    resultstree left;
    resultstree right;
};

/**
 * An error checking malloc function.
 *
 * @param s The size of the memory to be allocated.
 *
 * @return result A pointer to the allocated memory.
 */
static void *emalloc(size_t s) {
    void *result = malloc(s);
    
    if (NULL == result) {
        fprintf(stderr, "Memory allocation failure\n");
        exit(EXIT_FAILURE);
    }
    
    return result;
}


/**
 * Initiates search on a given string of search terms, setting up
 * variables and tokenising as necessary.
 *
 * @param terms The complete search query.
 */
void search(char *terms, FILE *lookupIn, FILE *postingsIn) {
    resultsFirstPass = NULL;
    
    lookup = lookupIn;
    postings = postingsIn;
    
    if (!lookup || !postings) {
        printf("Couldn't load index files");
        exit(EXIT_FAILURE);
    }
    
    char *searchTerms = strtok(terms, " ");
    while (searchTerms != NULL) {
        for (int i = 0; i < strlen(searchTerms); i++) {
            searchTerms[i] = tolower(searchTerms[i]);
        }
        if (searchTerms[strlen(searchTerms)-1] == '\n') {
            searchTerms[strlen(searchTerms)-1] = '\0';
        }

        get_term(searchTerms);
        fseek(lookup, 0, SEEK_SET);
        fseek(postings, 0, SEEK_SET);
        searchTerms = strtok (NULL, " ");
    }
    
    results_tree_inorder(resultsFirstPass, NULL, results_order_by_relevance);
    results_tree_inorder(resultsSecondPass, NULL, results_print);
}


/**
 * Finds and processes words matching the given search term.
 *
 * @param term The given search term.
 */
void get_term(char *term){
    char *output = emalloc(sizeof(char) * 20);
    int location = 0;
    int length = 0;
    int docno = 0;
    int occurrence = 0;
    size_t postingUnit = sizeof(int) * 2;
    
    /*### Variables for incomplete binary search implementations ###*/
//    size_t size;
//    long temp;
//    size_t lookupUnit = sizeof(char) * 20 + sizeof(int);
//    size_t lookupWord = sizeof(char) * 20;
//    long first;
//    long last;
//    long middle;
//    int cmp;
    
    /*### Linear Search - Placeholder to demonstrate other functionality ###*/
    while (1){
        output[0] = '\0';
        location = 0;
        length = 0;
        
        if (fread(output, sizeof(char) * 20, 1, lookup) == 0) break;
        fread(&location, sizeof(int), 1, lookup);
        fread(&length, sizeof(int), 1, lookup);
        
        if (strcmp(output, term) == 0) {
            fseek(postings, location, SEEK_SET);
            for (int i = 0; i < length; i += sizeof(int) * 2){
                fread(&docno, sizeof(int), 1, postings);
                fread(&occurrence, sizeof(int), 1, postings);

                resultsFirstPass = results_tree_insert_initial(resultsFirstPass, docno, (float)occurrence/((float)length/(float)postingUnit));
                
                docno = 0;
                occurrence = 0;
            }
        }
    }
   

    /*### Preparations needed for in-file binary search ###*/
//    fseek(lookup, 0L, SEEK_END);
//    size = ftell(lookup);
//    temp = size/lookupUnit;
//    fseek(lookup, 0L, SEEK_SET);
    
    
    /*### Binary Search 1 ###*/
//    first = 0;
//    last = temp - 1;
//    middle = (first+last)/2;
//    
//    while (first <= last) {
//        fseek(lookup, middle * lookupUnit, SEEK_SET);
//        fread(output, lookupWord, 1, lookup);
//        cmp = strcmp(term, output);
//
//        if (cmp > 0) {
//            first = middle + 1;
//        } else if (cmp == 0) {
//            fread(&location, sizeof(int), 1, lookup);
//            fread(&length, sizeof(int), 1, lookup);
//            
//            fseek(postings, location, SEEK_SET);
//            
//            for (int i = 0; i < length; i += sizeof(int) * 2) {
//                fread(&docno, sizeof(int), 1, postings);
//                fread(&occurrence, sizeof(int), 1, postings);
//
//                resultsFirstPass = results_tree_insert_initial(resultsFirstPass, docno, (float)occurrence/((float)length/(float)postingUnit));
//
//                docno = 0;
//                occurrence = 0;
//            }
//            break;
//            
//        } else last = middle - 1;
//        
//        middle = first + (last - first)/2;
//        
//        printf("f:%ld m:%ld l:%ld", first, middle, last);
//    }

    
    /*### Binary Search 2 ###*/
//    long newLoc = temp/2;
//    long scope = temp;
//    
//    while (scope > 1 && newLoc < temp && newLoc > 0) {
//        fseek(lookup, newLoc * lookupUnit, SEEK_SET);
//        fread(output, lookupWord, 1, lookup);
//        cmp = strcmp(term, output);
//        
//        if (cmp == 0) {
//            fread(&location, sizeof(int), 1, lookup);
//            fread(&length, sizeof(int), 1, lookup);
//            
//            fseek(postings, location, SEEK_SET);
//            
//            for (int i = 0; i < length; i += sizeof(int) * 2) {
//                fread(&docno, sizeof(int), 1, postings);
//                fread(&occurrence, sizeof(int), 1, postings);
//                
//                resultsFirstPass = results_tree_insert_initial(resultsFirstPass, docno, (float)occurrence/((float)length/(float)postingUnit));
//                
//                docno = 0;
//                occurrence = 0;
//            }
//
//            break;
//            
//        } else if (cmp < 0) {
//            newLoc -= newLoc/2;
//            
//        } else {
//            newLoc += newLoc/2;
//        }
//        
//        scope /= 2;
//    }
    
    free(output);
}

/**
 * Inserts a new node with a given document number into a given results tree,
 * sorting by document number and accumulating relevance for the contained
 * document numbers.
 *
 * @param b The tree that is to receive the key.
 * @param str The string that is being inserted as a key into
 *			the tree.
 *
 * @return The tree containing the new key/node.
 */
resultstree results_tree_insert_initial(resultstree b, int doc, float relevance) {
    if (NULL == b) {
        b = emalloc(sizeof *b);
        b->key = doc;
        b->rsv = relevance;
        b->left = NULL;
        b->right = NULL;
        
    } else {
        if (doc == b->key) {
            b->rsv += relevance;
            return b;
            
        } else if (doc < b->key) {
            b->left = results_tree_insert_initial(b->left, doc, relevance);
            
        } else if (doc > b->key) {
            b->right = results_tree_insert_initial(b->right, doc, relevance);
        }
    }
    
    return b;
}

/**
 * Inserts a new node with a given document number into a given results tree,
 * sorting by relevance score.
 *
 * @param b The tree that is to receive the key.
 * @param str The string that is being inserted as a key into
 *			the tree.
 *
 * @return The tree containing the new key/node.
 */
resultstree results_tree_insert_final(resultstree b, int doc, float relevance) {
    if (NULL == b) {
        b = emalloc(sizeof *b);
        b->key = doc;
        b->rsv = relevance;
        b->left = NULL;
        b->right = NULL;
        
    } else {
        if (relevance >= b->rsv) {
            b->left = results_tree_insert_final(b->left, doc, relevance);
            
        } else if (relevance < b->rsv) {
            b->right = results_tree_insert_final(b->right, doc, relevance);
        }
    }
    
    return b;
}

/**
 * Performs an in-order traversal of a given results tree, executing
 * a provided function on each node.
 *
 * @param b The root node of the tree being traversed.
 */
void results_tree_inorder (resultstree b, resultstree parent, void f(int doc, float relevance)) {
    while(b != NULL) {
        if (parent != NULL) {
            parent->left = b->right;
            b->right = parent;
        }
        
        if (b->left != NULL) {
            parent = b;
            b = b->left;
        } else {
            f(b->key, b->rsv);
            b = b->right;
            parent = NULL;
        }
    }
}

/**
 * Passes document data into a new tree to be organised by relevance.
 *
 * @param doc The document id being added.
 * @param relevance The documents relevance score.
 */
void results_order_by_relevance (int doc, float relevance) {
    resultsSecondPass = results_tree_insert_final(resultsSecondPass, doc, relevance);
}

/**
 * Formats and prints a compressed document number and its relevance score.
 *
 * @param doc The document number being printed
 * @param relevance The relevance score of that document number
 */
void results_print (int doc, float relevance) {
    char str[16];
    char str2[16];
    char str3[16];
    
    strcpy(str, "WSJ");
    sprintf(str2, "%d", doc);
    
    if (strlen(str2) == 7) {
        strcat(str, "900");
    } else if (strlen(str2) == 8) {
        strcat(str, "90");
    } else if (str2[0] == '1' || str2[0] == '2') {
        strcat(str, "9");
    } else if (str2[0] == '6' || str2[0] == '7' || str2[0] == '8' || str2[0] == '9') {
        strcat(str, "8");
    }
    
    strcat(str, str2);
    
    strncpy(str3, str, 9);
    str3[9] = '-';
    str3[10] = '\0';
    strcat(str3, str + 9);
    str3[14] = '\0';
    
    printf("%s %f\n", str3, relevance);
}