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
void CodeBlock();
void Relation();
void BoolExpression();

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

void EmitStringArrayDeclaration(char *def, char *name, int strSize, int arrSize)
{
  sprintf(tmp, "%s %s, %d, %d", def, name, strSize, arrSize);
  EmitLn(tmp);
}

void stringArrayDeclaration(char *def, char *value, int arraySize)
{
  char temp[BUFFER_SIZE];
  strcpy(temp,value);
  int stringSize = getStringSize();
  EmitStringArrayDeclaration(def, temp, stringSize, arraySize);
  next();
  matchString(")");
  int totalValue = stringSize * arraySize;
  insertSymbol(STRING, totalValue, temp);
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
      insertSymbol(INT, INT_SZ * arraySize, VALUE);
      break;
    case FLOAT_TYPE:
      EmitArrayDeclaration("defaf", VALUE, arraySize);
      insertSymbol(FLOAT, FLOAT_SZ * arraySize, VALUE);
      break;
    case DOUBLE_TYPE:
      EmitArrayDeclaration("defad", VALUE, arraySize);
      insertSymbol(DOUBLE, DOUBLE_SZ * arraySize, VALUE);
      break;
    case STRING_TYPE:
      stringArrayDeclaration("defas", VALUE, arraySize);
    default:
      break;
  }
}

void stringDeclaration(char* def, char* value) {
  char temp[BUFFER_SIZE];
  strcpy(temp,value);
  int stringSize = getStringSize();
  //using array declaration because it has the same format
  //it's basically defs name,10
  EmitArrayDeclaration(def, temp, stringSize);
  insertSymbol(STRING,stringSize,temp);
  next();
  matchString(")");
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
    case STRING_TYPE:
      stringDeclaration("defs", VALUE);
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

void LoadVar(char type, char* name) {
  switch(type) {
  case CHAR:
    sprintf(tmp, "pushc %s", name);
    break;
  case INT:
    sprintf(tmp, "pushi %s", name);
    break;
  case FLOAT:
    sprintf(tmp, "pushf %s", name);
    break;
  case DOUBLE:
    sprintf(tmp, "pushd %s", name);
    break;
  case STRING:
    sprintf(tmp, "pushs %s", name);
  }
  EmitLn(tmp);
}
void LoadArray(char type, char* name) {
  switch(type) {
  case CHAR:
    sprintf(tmp, "pushac %s", name);
    break;
  case INT:
    sprintf(tmp, "pushai %s", name);
    break;
  case FLOAT:
    sprintf(tmp, "pushaf %s", name);
    break;
  case DOUBLE:
    sprintf(tmp, "pushad %s", name);
    break;
  case STRING:
    sprintf(tmp, "pushas %s", name);
  }
  EmitLn(tmp);

}

void Load(char *Name) {
  struct symbol_row * variable = findVariable(Name);
  if(variable == NULL) error("Undefined variable");
  next();
  int isArray = 0;
  if(TOKEN == LEFT_BRACKET) {
    isArray = 1;
    next();
    Expression();
    EmitLn("popx");
    if(TOKEN != RIGHT_BRACKET) expected("closing array bracket");
    next();
  }
  if(isArray)LoadArray(variable->type, variable->name);
  else LoadVar(variable->type, variable->name);
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
    BoolExpression();
    if(TOKEN != RIGHT_PAREN) expected("closing exp paren");
    next();
  }
  else {
    if(TOKEN == NAME) {
      Load(VALUE);
    } else if(TOKEN == NUMBER || TOKEN == DECIMAL) {
      LoadConst(VALUE);
      next();
    }
    else {
      expected("Math Factor");
      next();
    }
  }
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

int isVarString(char *name) {
  struct symbol_row * variable = findVariable(name);
  if(variable == NULL) error("Undefined variable");
  if(variable->type == STRING)
    return 1;
  else
    return 0;
}

void AssignConstant() {
  //TODO: basically does pushks with the word i have
}

/*
  possible assignments are
  something = "constant";
  something = somethingElse
  something = somethingElse[i]
  something[i] = "constant"
  something[i] = somethingElse[i]
  something[i] = somethingElse
  ill start with the simple ones, i need to see what's up with arrays later
*/
void stringExpression() {
  if(TOKEN == QUOTE) {
    next();
    AssignConstant();//this should do pushks
    next();
    matchString("\"");
    next();//not sure if i need this nex
  }
  else {
    if(TOKEN == NAME) {
      Load(VALUE);
      next();//not sure if i need this next
    }
  }
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

void EmitComparison(char* jumpType) {
  char l1[BUFFER_SIZE];
  char l2[BUFFER_SIZE];
  genLabel(l1);
  genLabel(l2);
  sprintf(tmp, "%s %s", jumpType, l1);
  EmitLn(tmp);
  EmitLn("pushki 0");
  sprintf(tmp, "jmp %s", l2);
  EmitLn(tmp);
  EmitLabel(l1);
  EmitLn("pushki 1");
  EmitLabel(l2);
}

void Compare() {
  next();
  Expression();
  EmitLn("cmp");
}

void Equals() {
  Compare();
  EmitComparison("jmpeq");
}

void NotEqual() {
  Compare();
  EmitComparison("jmpne");
}

void Less() {
  Compare();
  EmitComparison("jmplt");
}

void LessEqual() {
  Compare();
  EmitComparison("jmple");
}

void Greater() {
  Compare();
  EmitComparison("jmpgt");
}

void GreaterEqual() {
  Compare();
  EmitComparison("jmpge");
}

void Relation() {
  Expression();
  if(isRelop(TOKEN)) {
    switch(TOKEN) {
      case EQUAL: Equals(); break;
      case LESS: Less(); break;
      case LESS_EQUAL: LessEqual(); break;
      case GREATER: Greater(); break;
      case GREATER_EQUAL: GreaterEqual(); break;
      case NOT_EQUAL: NotEqual(); break;
    }
  }
}

void NotIt() {
  char l1[BUFFER_SIZE];
  char l2[BUFFER_SIZE];
  genLabel(l1);
  genLabel(l2);
  EmitLn("pushki 0");
  EmitLn("cmp");
  sprintf(tmp, "jmpeq %s", l1);
  EmitLn(tmp);
  EmitLn("pushki 0");
  sprintf(tmp, "jmp %s", l2);
  EmitLn(tmp);
  EmitLabel(l1);
  EmitLn("pushki 1");
  EmitLabel(l2);
}

void NotFactor() {
  if(TOKEN == NOT) {
    next();
    Relation();
    NotIt();
  }
  else Relation();
}

void DoAnd() {
  EmitLn("mul");
}

void BoolTerm() {
  NotFactor();
  while(TOKEN == AND) {
    next();
    NotFactor();
    DoAnd();
  }
}

void DoOr() {
  EmitLn("add");
}

void BoolExpression() {
  BoolTerm();
  while(TOKEN == OR) {
    next();
    BoolTerm();
    DoOr();
  }
}

void StoreVar(char* name) {
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

  if(variable->type == STRING)
    sprintf(tmp, "pops %s", name);

  EmitLn(tmp);
}

void StoreArray(char* name) {
  struct symbol_row * variable = findVariable(name);
  if(variable == NULL) error("variable not found");
  if(variable->type == CHAR)
    sprintf(tmp, "popac %s", name);

  if(variable->type == INT)
    sprintf(tmp, "popai %s", name);

  if(variable->type == FLOAT)
    sprintf(tmp, "popaf %s", name);

  if(variable->type == DOUBLE)
    sprintf(tmp, "popad %s", name);

  if(variable->type == STRING)
    sprintf(tmp, "popas %s", name);

  EmitLn(tmp);
}

void DoAssignment() {
  char name[BUFFER_SIZE];
  strcpy(name, VALUE);
  next();
  int isArray = 0;
  if(TOKEN == LEFT_BRACKET) {
    isArray = 1;
    next();
    Expression();
    EmitLn("popy");
    if(TOKEN != RIGHT_BRACKET) expected("closing array");
    next();
  }
  matchString("=");
  next();
  if(isVarString(name))
    stringExpression();
  else
    BoolExpression();
  if(isArray) {
    EmitLn("movy");
    StoreArray(name);
  }
  else StoreVar(name);
}

void PrintVar(char type, char *name) {
  switch(type) {
  case CHAR:
    sprintf(tmp, "prtc %s", name);
    break;
  case INT:
    sprintf(tmp, "prti %s", name);
    break;
  case FLOAT:
    sprintf(tmp, "prtf %s", name);
    break;
  case DOUBLE:
    sprintf(tmp, "prtd %s", name);
    break;
  case STRING:
    sprintf(tmp, "prts %s", name);
  }
  EmitLn(tmp);

}

void PrintArray(char type, char *name) {
  switch(type) {
  case CHAR:
    sprintf(tmp, "prtac %s", name);
    break;
  case INT:
    sprintf(tmp, "prtai %s", name);
    break;
  case FLOAT:
    sprintf(tmp, "prtaf %s", name);
    break;
  case DOUBLE:
    sprintf(tmp, "prtad %s", name);
    break;
  case STRING:
    sprintf(tmp, "prtas %s", name);
  }
  EmitLn(tmp);
}
void Print(char* Name) {
  struct symbol_row *variable = findVariable(Name);
  if(variable == NULL) error("Undefined variable");
  next();
  int isArray = 0;
  if(TOKEN == LEFT_BRACKET) {
    isArray = 1;
    next();
    Expression();
    EmitLn("popx");
    if(TOKEN != RIGHT_BRACKET) expected("closing array");
    next();
  }
  if(isArray) PrintArray(variable->type, variable->name);
  else PrintVar(variable->type, variable->name);
  EmitLn("prtcr");
}

void DoPrint() {
  next();
  if(TOKEN != LEFT_PAREN) expected("opening paretheses");
  next();
  if(TOKEN != NAME) expected("variable to print");
  Print(VALUE);
  while(TOKEN == COMMA) {
    next();
    if(TOKEN != NAME) expected("variable to print");
    Print(VALUE);
  }
  if(TOKEN != RIGHT_PAREN) expected("closing paretheses");
  next();

}

void ReadVar(char type, char *name) {
  switch(type) {
  case CHAR:
    sprintf(tmp, "rdc %s", name);
    break;
  case INT:
    sprintf(tmp, "rdi %s", name);
    break;
  case FLOAT:
    sprintf(tmp, "rdf %s", name);
    break;
  case DOUBLE:
    sprintf(tmp, "rdd %s", name);
    break;
  case STRING:
    sprintf(tmp, "rds %s", name);
  }
  EmitLn(tmp);
}

void ReadArray(char type, char *name) {
  switch(type) {
  case CHAR:
    sprintf(tmp, "rdac %s", name);
    break;
  case INT:
    sprintf(tmp, "rdai %s", name);
    break;
  case FLOAT:
    sprintf(tmp, "rdaf %s", name);
    break;
  case DOUBLE:
    sprintf(tmp, "rdad %s", name);
    break;
  case STRING:
    sprintf(tmp, "rdas %s", name);
  }
  EmitLn(tmp);
}

void Read(char* Name) {
  struct symbol_row * variable = findVariable(Name);
  if(variable == NULL) error("Undefined variable");
  next();
  int isArray = 0;
  if(TOKEN == LEFT_BRACKET) {
    isArray = 1;
    next();
    Expression();
    EmitLn("popx");
    if(TOKEN != RIGHT_BRACKET) expected("closing array");
    next();
  }
  if(isArray) ReadArray(variable->type, variable->name);
  else ReadVar(variable->type, variable->name);

}

void DoRead() {
  next();
  if(TOKEN != LEFT_PAREN) expected("opening paretheses");
  next();
  if(TOKEN != NAME) expected("variable to read");
  Read(VALUE);

  while(TOKEN == COMMA) {
    next();
    if(TOKEN != NAME) expected("variable to read");
    Read(VALUE);
  }
  if(TOKEN != RIGHT_PAREN) expected("closing paretheses");
  next();

}

void DoIf() {
  next();
  if(TOKEN != LEFT_PAREN) expected("opening paretheses");
  next();
  BoolExpression();
  EmitLn("pushki 0");
  EmitLn("cmp");
  if(TOKEN != RIGHT_PAREN) expected("closing paretheses");
  next();
  char l1[BUFFER_SIZE];
  genLabel(l1);
  sprintf(tmp, "jmpeq %s", l1);
  EmitLn(tmp);
  CodeBlock();
  EmitLabel(l1);
}

void DoWhile() {
  char l0[BUFFER_SIZE];
  genLabel(l0);
  EmitLabel(l0);
  next();
  if(TOKEN != LEFT_PAREN) expected("opening paretheses");
  next();
  BoolExpression();
  EmitLn("pushki 0");
  EmitLn("cmp");
  if(TOKEN != RIGHT_PAREN) expected("closing paretheses");
  next();
  char l1[BUFFER_SIZE];
  genLabel(l1);
  sprintf(tmp, "jmpeq %s", l1);
  EmitLn(tmp);
  CodeBlock();
  sprintf(tmp, "jmp %s", l0);
  EmitLn(tmp);
  EmitLabel(l1);
}

void DoFor() {
  next();
  if(TOKEN != LEFT_PAREN) expected("opening paretheses");
  next();
  if(TOKEN == NAME) DoAssignment();
  if(TOKEN != SEMI_COLON) expected("semicolon");
  next();
  char l0[BUFFER_SIZE];
  genLabel(l0);
  char l1[BUFFER_SIZE];
  genLabel(l1);
  char l2[BUFFER_SIZE];
  genLabel(l2);
  char l3[BUFFER_SIZE];
  genLabel(l3);

  EmitLabel(l0);
  BoolExpression();
  EmitLn("pushki 0");
  EmitLn("cmp");
  sprintf(tmp, "jmpeq %s", l3);
  EmitLn(tmp);
  sprintf(tmp, "jmp %s", l2);
  EmitLn(tmp);
  if(TOKEN != SEMI_COLON) expected("semicolon");
  next();
  EmitLabel(l1);
  if(TOKEN == NAME) DoAssignment();
  sprintf(tmp, "jmp %s", l0);
  EmitLn(tmp);
  if(TOKEN != RIGHT_PAREN) expected("closing paretheses");
  next();
  EmitLabel(l2);
  CodeBlock();
  sprintf(tmp, "jmp %s", l1);
  EmitLn(tmp);
  EmitLabel(l3);
}

void Statement() {
  if(isDeclaration(TOKEN)) {
    DoDeclaration();
  }
  else if(isAssignment(TOKEN)) {
    DoAssignment();
  }
  else if(TOKEN == IF) {
    DoIf();
  }
  else if(TOKEN == PRINT) {
    DoPrint();
  }
  else if(TOKEN == READ) {
    DoRead();
  }
  else if(TOKEN == WHILE) {
    DoWhile();
  }
  else if(TOKEN == FOR) {
    DoFor();
  }
  else {
    error("unrecognized statement");
  }
}

void CodeLine() {
  Statement();
  while(TOKEN == SEMI_COLON) {
    next();
    Statement();
  }
}

void CodeBlock() {
  if(TOKEN != LEFT_BRACE) expected("{");
  next();
  while(TOKEN != RIGHT_BRACE) {
    CodeLine();
  }
  if(TOKEN != RIGHT_BRACE) expected("}");
  next();
}

void Language() {
  next();
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
