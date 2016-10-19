#include "compiler.h"
#include "code_gen.h"
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
      insertSymbol(CHAR, CHAR_SZ * arraySize, VALUE);
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
      insertSymbol(CHAR, CHAR_SZ, VALUE);
      break;
    case INT_TYPE:
      EmitDeclaration("defi", VALUE);
      insertSymbol(INT, INT_SZ, VALUE);
      break;
    case FLOAT_TYPE:
      EmitDeclaration("deff", VALUE);
      insertSymbol(FLOAT, FLOAT_SZ, VALUE);
      break;
    case DOUBLE_TYPE:
      EmitDeclaration("defd", VALUE);
      insertSymbol(DOUBLE, DOUBLE_SZ, VALUE);
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

void LoadVar(char * Name) {
  struct symbol_row * variable = findVariable(Name);
  if(variable == NULL) error("Undefined variable");
  switch(variable->type) {
  case CHAR:
    sprintf(tmp, "pushc %s", variable->name);
    break;
  case INT:
    sprintf(tmp, "pushi %s", variable->name);
    break;
  case FLOAT:
    sprintf(tmp, "pushf %s", variable->name);
    break;
  case DOUBLE:
    sprintf(tmp, "pushd %s", variable->name);
    break;
  }
  EmitLn(tmp);
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
  struct symbol_row * variable = findVariable(name);
  if(variable == NULL) error("variable not found");

  if(variable->type == CHAR)
    sprintf(tmp, "popc %s", name);

  if(variable->type == INT)
    sprintf(tmp, "popi %s", name);

  if(variable->type == FLOAT)
    sprintf(tmp, "popf %s", name);

  if(variable->type == DOUBLE)
    sprintf(tmp, "popd %s", name);

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
