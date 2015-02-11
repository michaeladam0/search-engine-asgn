/**
 * @file search.h
 * @author Michael Adam
 * @date April 2014
 */

#ifndef SEARCH_H_
#define SEARCH_H_

typedef struct results_tree_node *resultstree;

extern void search(char *terms, FILE *lookupIn, FILE *postingsIn);

void get_term(char *term);
resultstree results_tree_insert_initial (resultstree b, int doc, float relevance);
resultstree results_tree_insert_final (resultstree b, int doc, float relevance);
void results_tree_inorder (resultstree b, resultstree parent, void f(int doc, float relevance));
void results_order_by_relevance(int doc, float relevance);
void results_print(int doc, float relevance);

#endif