#include "compiler.h"

int isDeclaration(enum TOKENS TOKEN) {
  return TOKEN == INT_TYPE ||
         TOKEN == CHAR_TYPE ||
         TOKEN == FLOAT_TYPE ||
         TOKEN == DOUBLE_TYPE;
}

int isAssignment(enum TOKENS TOKEN) {
  return TOKEN == NAME;
}

int isRelop(enum TOKENS TOKEN) {
  return TOKEN >= LESS && TOKEN <= NOT_EQUAL;
}
