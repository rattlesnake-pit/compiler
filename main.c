#include "compiler.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

void Expression();

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
  next();
}

void LoadVar() {
}

void LoadConst(char* VALUE) {
  if(TOKEN == NUMBER)
    sprintf(tmp, "pushki %s", VALUE);
  else if(TOKEN == DECIMAL)
    sprintf(tmp, "pushkf %s", VALUE);
  EmitLn(tmp);
}

void Factor() {
  if(TOKEN == LEFT_PAREN) {
    next();
    Expression();
    matchString(")");
  }
  else {
    if(TOKEN == NAME) {
      LoadVar(VALUE);
    } else if(TOKEN == NUMBER || TOKEN == DECIMAL) {
      LoadConst(VALUE);
    }
    else {
      expected("Math Factor");
    }
  }
  next();
}

void Multiply() {
  next();
  Factor();
  EmitLn("mul");
}

void Divide() {
  next();
  Factor();
  EmitLn("div");
}

void MulExpression() {
  Factor();
  while(TOKEN == MUL || TOKEN == DIV) {
    switch(TOKEN) {
      case MUL: Multiply(); break;
      case DIV: Divide(); break;
    }
  }
}

void Add() {
  next();
  MulExpression();
  EmitLn("add");
}

void Substract() {
  next();
  MulExpression();
  EmitLn("sub");
}

void Expression() {
  MulExpression();
  while(TOKEN == ADD || TOKEN == SUB) {
    switch(TOKEN) {
      case ADD : Add(); break;
      case SUB : Substract(); break;
    }
  }
}

void Store(char* name) {
  sprintf(tmp, "popi %s", name);
  EmitLn(tmp);
}

void DoAssignment() {
  char name[BUFFER_SIZE];
  strcpy(name, VALUE);
  next();
  matchString("=");
  next();
  Expression();
  Store(name);
  
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
  if(TOKEN == ENDLINE) {
    next();
  }
  else {
    Statement();
  while(TOKEN == SEMI_COLON) {
    next();
    Statement();
  }
   matchString("\n");
  }
}

void CodeBlock() {
  next();
  matchString("{");
  next();
  while(TOKEN != RIGHT_BRACE){
    CodeLine();
  }
  matchString("}");
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
