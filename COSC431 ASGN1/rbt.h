/**
 * @file rbt.h
 * @author Michael Adam
 * @date April 2014
 */
 
//#include <stdio.h> /* To allow for the use of FILE */

#ifndef RBT_H_
#define RBT_H_

typedef struct tree_node *tree;
typedef struct posting_node *posting;

typedef enum { RED, BLACK } tree_colour;

extern tree root_node;

extern tree tree_free (tree b);
extern tree tree_insert (tree b, char const *str, int doc);
extern void tree_write_to_file (tree b);

posting store_docno (posting post, int doc);
posting posting_free (posting docs);
void tree_inorder (tree b, tree parent);
void tree_output (char *str, posting docs);

#endif
