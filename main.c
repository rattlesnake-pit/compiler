#include "compiler.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

void EmitLn(char* s) {
  printf("%s\n", s);
}

void EmitDeclaration(char* def, char* name) {
  sprintf(tmp, "%s %s", def, name);
  EmitLn(tmp);
}

void EmitArrayDeclaration(char* def, char* name, int size) {
  sprintf(tmp, "%s %s, %d", def, name, size);
  EmitLn(tmp);
}

void DoArrayDeclaration(enum TOKENS type) {
  next();
  if(TOKEN != NUMBER) error("expected array size");

  int arraySize = atoi(VALUE);

  next();
  matchString("]");
  next();

  if(TOKEN != NAME) error("expected array name");
  switch(type) {
    case CHAR_TYPE:
      EmitArrayDeclaration("defac", VALUE, arraySize);
      break;
    case INT_TYPE:
      EmitArrayDeclaration("defai", VALUE, arraySize);
      break;
    case FLOAT_TYPE:
      EmitArrayDeclaration("defaf", VALUE, arraySize);
      break;
    case DOUBLE_TYPE:
      EmitArrayDeclaration("defad", VALUE, arraySize);
      break;
    default:
      break;
  }
}

void DoVariableDeclaration(enum TOKENS type) {
  switch(type) {
    case CHAR_TYPE:
      EmitDeclaration("defc", VALUE);
      break;
    case INT_TYPE:
      EmitDeclaration("defi", VALUE);
      break;
    case FLOAT_TYPE:
      EmitDeclaration("deff", VALUE);
      break;
    case DOUBLE_TYPE:
      EmitDeclaration("defd", VALUE);
      break;
    default:
      break;
  }
}

void DoDeclaration() {
  enum TOKENS type = TOKEN;
  next();
  if(TOKEN == LEFT_BRACKET) {
    DoArrayDeclaration(type);
  }
  else if(TOKEN == NAME) {
    DoVariableDeclaration(type);
  }
  else {
    error("unexpected Declaration");
  }
}

void DoAssignment() {

}
void Statement() {
  if(isDeclaration(TOKEN)) {
    DoDeclaration();
  }
  else if(isAssignment(TOKEN)) {
    DoAssignment();
  }
  else {
    error("unrecognized statement");
  }
}

void CodeLine() {
  do {
    Statement();
    next();
  } while(TOKEN == SEMI_COLON);
  matchString("\n");
  next();
}

void CodeBlock() {
  next();
  matchString("{");
  next();
  while(TOKEN != RIGHT_BRACE){
    if(isSpecificString("\n")){
      CodeLine();
    }
    else{
      matchString("\n");
      next();
    }
  }
}

void Language() {
  CodeBlock();
}

int main() {
#ifdef _WIN32
  _setmode( _fileno(stdout), _O_BINARY);
#endif
  init();
  Language();
  return 0;
}
