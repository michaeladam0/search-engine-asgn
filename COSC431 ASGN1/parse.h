/**
 * @file parse.h
 * @author Michael Adam
 * @date April 2014
 */

#include <stdio.h>
#include "index.h"

#ifndef PARSE_H_
#define PARSE_H_

void parse(FILE *stream);
void end_word(void);
void add_to_word(char c);

#endif
