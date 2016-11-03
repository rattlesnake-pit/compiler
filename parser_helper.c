#include "compiler.h"
#include "stdio.h"
#include "stdlib.h"

int getStringSize()
{
  next();
  if(TOKEN == LEFT_PAREN)
  {
    next();
    if(TOKEN != NUMBER) error("expected string size");
    int size = atoi(VALUE);
    return size;
  }
  error("no parenthesis");
}

int isDeclaration(enum TOKENS TOKEN) {
  return TOKEN == INT_TYPE ||
         TOKEN == CHAR_TYPE ||
         TOKEN == FLOAT_TYPE ||
         TOKEN == DOUBLE_TYPE ||
         TOKEN == STRING_TYPE;
}

int isAssignment(enum TOKENS TOKEN) {
  return TOKEN == NAME;
}

int isRelop(enum TOKENS TOKEN) {
  return TOKEN >= LESS && TOKEN <= NOT_EQUAL;
}
