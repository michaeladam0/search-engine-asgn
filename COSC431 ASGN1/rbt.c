/**
 * @file tree.c
 * @author Michael Adam
 * @date April 2014
 * 
 * A multipurpose tree class
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rbt.h"

/* Macro Definitions */
#define IS_BLACK(x) ((NULL == (x)) || (BLACK == (x)->colour))
#define IS_RED(x) ((NULL != (x)) && (RED == (x)->colour))

/* Variable declarations */
tree root_node;
FILE *postings_output_stream;
FILE *lookup_output_stream;
int postings_count;
int postings_location;

/* Struct Definitions */
struct tree_node {
    char *key;
    posting docs;
    tree left;
    tree right;
    
    tree_colour colour;
};

struct posting_node {
    int docno;
    int occurrence;
    posting next;
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
 * Performs a right rotation on a given root node to maintain
 * red black properties.
 *
 * @param b The node that is to be rotated.
 *
 * @return The new root node resulting from the rotation.
 */
static tree right_rotate(tree b) {
  tree temp = b;

  if (b == root_node) root_node = b->left;

  b = b->left;
  temp->left = b->right;
  b->right = temp;
  
  return b;
}

/**
 * Performs a left rotation on a given root node to maintain
 * red black properties.
 *
 * @param b The node that is to be rotated.
 *
 * @return The new root node resulting from the rotation.
 */
static tree left_rotate(tree b) {
  tree temp = b;
  
  if (b == root_node) root_node = b->right;

  b = b->right;
  temp->right = b->left;
  b->left = temp;
  
  return b;
}

/**
 * Executes appropriate rotation and colour fixing operations
 * on a red black tree to ensure that it doesn't violate
 * red-black properties.
 *
 * @param b The node to be checked.
 *
 * @return The new root node resulting from any fixes that
 *			have been applied.
 */
static tree tree_fix(tree b) {
  if (IS_RED(b->left)) {
    if (IS_RED(b->left->left)) {
      if (IS_RED(b->right)) {
		b->colour = RED;
		b->left->colour = BLACK;
		b->right->colour = BLACK;
      } else if (IS_BLACK(b->right)) {
		b = right_rotate(b);
		b->colour = BLACK;
		b->right->colour = RED;
      }
    }
    
    if (IS_RED(b->left->right)) {
      if (IS_RED(b->right)) {
		b->colour = RED;
		b->left->colour = BLACK;
		b->right->colour = BLACK;
      } else if (IS_BLACK(b->right)) {
		b->left = left_rotate(b->left);
		b = right_rotate(b);
		b->colour = BLACK;
		b->right->colour = RED;
      }
    }
  }

  if (IS_RED(b->right)) {
    if (IS_RED(b->right->left)) {
      if (IS_RED(b->left)) {
		b->colour = RED;
		b->left->colour = BLACK;
		b->right->colour = BLACK;
      } else if (IS_BLACK(b->left)) {
		b->right = right_rotate(b->right);
		b = left_rotate(b);
		b->colour = BLACK;
		b->left->colour = RED;
      }
    }
    
    if (IS_RED(b->right->right)) {
      if (IS_RED(b->left)) {
		b->colour = RED;
		b->left->colour = BLACK;
		b->right->colour = BLACK;
      } else if (IS_BLACK(b->left)) {
		b = left_rotate(b);
		b->colour = BLACK;
		b->left->colour = RED;
      }
    }
  }
  
  root_node->colour = BLACK;
  
  return b;
}

/**
 * Frees any dynamic memory that has been allocated to the tree.
 *
 * @param b The root node of the tree being freed from memory.
 *
 * @return NULL can be used by the calling function to overwrite
 *           the link to the previous node, preventing memory issues.
 */
tree tree_free(tree b) {
  if (NULL == b) {
    return b;
  }
  
  tree_free(b->left);
  tree_free(b->right);

  free(b->key);
  posting_free(b->docs);
  free(b);

  return NULL;
}

/**
 * Frees any dynamic memory that has been allocated to a posting or its children.
 *
 * @param posting The root node of the list being freed from memory
 *
 * @return NULL can be used by the calling function to overwrite
 *           the link to the previous node, preventing memory issues.
 */
posting posting_free(posting docs){
    if (NULL == docs) {
        return docs;
    }
    
    posting_free(docs->next);
    free(docs);
    
    return NULL;
}

/**
 * Inserts a new node with a given key into a given tree,
 * and executes fixing operations to maintain red/black properties.
 *
 * @param b The tree that is to receive the key.
 * @param str The string that is being inserted as a key into
 *			the tree.
 *
 * @return The tree containing the new key/node.
 */
tree tree_insert(tree b, char const *str, int doc) {
    int cmp;

    if (NULL == b) {
        b = emalloc(sizeof *b);
        b->key = emalloc((strlen(str)+1) * sizeof str[0]);
        strcpy(b->key, str);
        b->left = NULL;
        b->right = NULL;
        
        b->docs = emalloc(sizeof(posting));
        b->docs->docno = doc;
        b->docs->occurrence = 1;
        b->docs->next = NULL;
        
        b->colour = RED;
        if (NULL == root_node) root_node = b;

    } else {
        cmp = strcmp(str, b->key);

        if (cmp == 0) {
            b->docs = store_docno(b->docs, doc);
            return b;

        } else if (cmp < 0) {
            b->left = tree_insert(b->left, str, doc);

        } else if (cmp > 0) {
            b->right = tree_insert(b->right, str, doc);
        }
    }

    b = tree_fix(b);

    return b;
}

/**
 * Stores document numbers and increments their associated word frequency
 * within a singly linked list of posting nodes.
 *
 * @param post The root posting node
 * @param doc The document number being stored.
 *
 * @return result A pointer to the (possibly new) root node.
 */
posting store_docno(posting post, int doc){
    posting newpost;
    
    if (post->docno == doc) {
        post->occurrence ++;
        
        return post;
        
    } else {
        newpost = emalloc(sizeof(posting));
        newpost->docno = doc;
        newpost->occurrence = 1;
        newpost->next = post;
        
        return newpost;
    }
}

/**
 * Receives an index tree and sets up the variables necessary to 
 * save it to disc, before calling the inorder traversal method
 * to begin saving data.
 *
 * @param b The tree being saved.
 */
void tree_write_to_file(tree b){
    lookup_output_stream = fopen("./lookup.bin", "wb");
    postings_output_stream = fopen("./postings.bin", "wb");
    postings_location = 0;
    
    if (!lookup_output_stream || !postings_output_stream) {
        printf("Unable to open file!");
        exit(EXIT_FAILURE);
    }
    
    tree_inorder(b, NULL);
    
    fclose(lookup_output_stream);
    fclose(postings_output_stream);
}

/**
 * Writes a given string and list of postings to the lookup and
 * postings files respectively, generating associated metadata as
 * it goes.
 *
 * @param str The word being added to lookup.
 * @param docs The document numbers being stored to postings.
 */
void tree_output(char *str, posting docs){
    posting temp;
    temp = docs;
    
    postings_count = 0;
    
    fwrite(str, sizeof(char)*20, 1, lookup_output_stream);
    fwrite(&postings_location, sizeof(int), 1, lookup_output_stream);
    
    while (temp != NULL) {
        fwrite(&temp->docno, sizeof(int), 1, postings_output_stream);
        fwrite(&temp->occurrence, sizeof(int), 1, postings_output_stream);
        postings_count += sizeof(int) * 2;
        
        temp = temp->next;
    }
    
    fwrite(&postings_count, sizeof(int), 1, lookup_output_stream);

    postings_location += postings_count;
    free(temp);
}

/**
 * Performs an iterative traversal of the given search tree,
 * calling the tree_output method as each node is reached in order,
 * so that each node is saved to disc.
 *
 * @param b The tree being traversed
 * @param doc The parent of that tree // Redundant?
 */
void tree_inorder (tree b, tree parent) {
    while (b != NULL) {
        if (parent != NULL) {
            parent->left = b->right;
            b->right = parent;
        }
        
        if (b->left != NULL) {
            parent = b;
            b = b->left;
            
        } else {
            tree_output(b->key, b->docs);
            b = b->right;
            parent = NULL;
        }
    }
}
