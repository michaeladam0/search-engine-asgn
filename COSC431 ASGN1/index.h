/**
 * @file index.h
 * @author Michael Adam
 * @date April 2014
 */

#include "rbt.h"

#ifndef INDEX_H_
#define INDEX_H_

extern void begin_indexing(void);
extern void end_indexing(void);
extern void start_tag(char const *);
extern void end_tag(char const *);
extern void word(char const *);

#endif