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
  if(type_out ==  ASM)
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
  insertSymbol2(STRING, totalValue, temp, stringSize);
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
      break;
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
    doVarOp(12,name);
    sprintf(tmp, "pushc %s", name);
    break;
  case INT:
    doVarOp(13,name);
    sprintf(tmp, "pushi %s", name);
    break;
  case FLOAT:
    doVarOp(14,name);
    sprintf(tmp, "pushf %s", name);
    break;
  case DOUBLE:
    doVarOp(15,name);
    sprintf(tmp, "pushd %s", name);
    break;
  case STRING:
    doVarOp(16,name);
    sprintf(tmp, "pushs %s", name);
  }
  EmitLn(tmp);
}
void LoadArray(char type, char* name) {
  switch(type) {
  case CHAR:
    doVarOp(17,name);
    sprintf(tmp, "pushac %s", name);
    break;
  case INT:
    doVarOp(18,name);
    sprintf(tmp, "pushai %s", name);
    break;
  case FLOAT:
    doVarOp(19,name);
    sprintf(tmp, "pushaf %s", name);
    break;
  case DOUBLE:
    doVarOp(20,name);
    sprintf(tmp, "pushad %s", name);
    break;
  case STRING:
    doVarOp(21,name);
    sprintf(tmp, "pushas %s", name);
  }
  EmitLn(tmp);

}

void LoadString(char *Name) {
  struct symbol_row * variable = findVariable(Name);
  if(variable == NULL) error("Undefined variable");
  int isArray = 0;
  next();
  if(TOKEN == LEFT_BRACKET) {
    isArray = 1;
    next();
    Expression();
    int stringSize = variable->stringSize;
    pushkiDirect(stringSize);
    sprintf(tmp,"pushki %d",stringSize);
    EmitLn(tmp);
    doOneByteOp(61);
    EmitLn("MUL");
    doOneByteOp(32);
    EmitLn("popx");
    if(TOKEN != RIGHT_BRACKET) expected("closing array bracket");
    next();
  }
  if(isArray)LoadArray(variable->type, variable->name);
  else LoadVar(variable->type, variable->name);
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
    doOneByteOp(32);
    EmitLn("popx");
    if(TOKEN != RIGHT_BRACKET) expected("closing array bracket");
    next();
  }
  if(isArray)LoadArray(variable->type, variable->name);
  else LoadVar(variable->type, variable->name);
}

void LoadConst(char* VALUE) {
  char temp[256];
  if(TOKEN == NUMBER) {
    strcpy(temp,VALUE);
    pushki(temp);
    sprintf(tmp, "pushki %s", VALUE);
  }
  else if(TOKEN == DECIMAL) {
    strcpy(temp,VALUE);
    pushkf(temp);
    sprintf(tmp, "pushkf %s", VALUE);
  }
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
  doOneByteOp(61);
  EmitLn("mul");
}

void Divide() {
  next();
  Factor();
  doOneByteOp(62);
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
  doOneByteOp(59);
  EmitLn("add");
}

void Substract() {
  next();
  MulExpression();
  doOneByteOp(60);
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
  if(strlen(VALUE) > 256){expected("string is too long");}
  char temp[256];
  strcpy(temp,VALUE);
  pushks(temp);
  sprintf(tmp,"pushks \"%s\"", VALUE);
  EmitLn(tmp);
}

void stringExpression() {
  if(TOKEN == QUOTE) {
    nextString();
    AssignConstant();//this should do pushks
    next();
    matchString("\"");
    next();
  }
  else {
    if(TOKEN == NAME) {
      LoadString(VALUE);
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

int getJumpType(char *jumpType) {
  if(!strcmp(jumpType,"jmpeq"))
    return 49;
  if(!strcmp(jumpType,"jmpne"))
    return 50;
    if(!strcmp(jumpType,"jmpgt"))
    return 51;
  if(!strcmp(jumpType,"jmpge"))
    return 52;
  if(!strcmp(jumpType,"jmplt"))
    return 53;
  if(!strcmp(jumpType,"jmple"))
    return 54;
  return -1;
}

void EmitComparison(char* jumpType) {
  char l1[BUFFER_SIZE];
  char l2[BUFFER_SIZE];
  genLabel(l1);
  genLabel(l2);
  int jtype = getJumpType(jumpType);
  doOneByteOp(jtype);
  doJump(l1);
  sprintf(tmp, "%s %s", jumpType, l1);
  EmitLn(tmp);
  pushki("0");
  EmitLn("pushki 0");
  doOneByteOp(48);
  doJump(l2);
  sprintf(tmp, "jmp %s", l2);
  EmitLn(tmp);
  EmitLabel(l1);
  pushki("1");
  EmitLn("pushki 1");
  EmitLabel(l2);
}

void Compare() {
  next();
  Expression();
  doOneByteOp(64);
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
  pushki("0");
  EmitLn("pushki 0");
  doOneByteOp(64);
  EmitLn("cmp");
  doOneByteOp(49);
  doJump(l1);
  sprintf(tmp, "jmpeq %s", l1);
  EmitLn(tmp);
  pushki("0");
  EmitLn("pushki 0");
  doOneByteOp(48);
  doJump(l2);
  sprintf(tmp, "jmp %s", l2);
  EmitLn(tmp);
  EmitLabel(l1);
  pushki("1");
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
  doOneByteOp(61);
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
  doOneByteOp(59);
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

  if(variable->type == CHAR) {
    sprintf(tmp, "popc %s", name);
    doVarOp(27,name);
  }

  if(variable->type == INT) {
    sprintf(tmp, "popi %s", name);
    doVarOp(28,name);
  }

  if(variable->type == FLOAT) {
    sprintf(tmp, "popf %s", name);
    doVarOp(29,name);
  }

  if(variable->type == DOUBLE) {
    sprintf(tmp, "popd %s", name);
    doVarOp(30,name);
  }

  if(variable->type == STRING){
    sprintf(tmp, "pops %s", name);
    doVarOp(31,name);
  }

  EmitLn(tmp);
}

void StoreArray(char* name) {
  struct symbol_row * variable = findVariable(name);
  if(variable == NULL) error("variable not found");
  if(variable->type == CHAR) {
    sprintf(tmp, "popac %s", name);
    doVarOp(33,name);
  }

  if(variable->type == INT) {
    sprintf(tmp, "popai %s", name);
    doVarOp(34,name);
  }

  if(variable->type == FLOAT) {
    sprintf(tmp, "popaf %s", name);
    doVarOp(35,name);
  }

  if(variable->type == DOUBLE) {
    sprintf(tmp, "popad %s", name);
    doVarOp(36,name);
  }

  if(variable->type == STRING) {
    sprintf(tmp, "popas %s", name);
    doVarOp(37,name);
  }

  EmitLn(tmp);
}

int obtainStringSize(char *name) {
  struct symbol_row * variable = findVariable(name);
  if(variable == NULL) error("variable not found");
  if(variable->type != STRING) error("wrong type");
  return variable->stringSize;
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
    if(isVarString(name)) {
      int strSize = obtainStringSize(name);
      pushkiDirect(strSize);
      sprintf(tmp,"pushki %d",strSize);
      EmitLn(tmp);
      doOneByteOp(61);
      EmitLn("MUL");
    }
    doOneByteOp(67);
    EmitLn("popy");
    if(TOKEN != RIGHT_BRACKET) expected("closing array");
    next();
  }
  matchString("=");
  next();
  if(isVarString(name)){
    stringExpression();
  }
  else
    BoolExpression();
  if(isArray) {
    doOneByteOp(66);
    EmitLn("movy");
    StoreArray(name);
  }
  else {
    StoreVar(name);
  }
}

void PrintVar(char type, char *name) {
  switch(type) {
  case CHAR:
    sprintf(tmp, "prtc %s", name);
    doVarOp(2,name);
    break;
  case INT:
    doVarOp(3,name);
    sprintf(tmp, "prti %s", name);
    break;
  case FLOAT:
    doVarOp(4,name);
    sprintf(tmp, "prtf %s", name);
    break;
  case DOUBLE:
    doVarOp(5,name);
    sprintf(tmp, "prtd %s", name);
    break;
  case STRING:
    doVarOp(6,name);
    sprintf(tmp, "prts %s", name);
  }
  EmitLn(tmp);

}

void PrintArray(char type, char *name) {
  switch(type) {
  case CHAR:
    doVarOp(7,name);
    sprintf(tmp, "prtac %s", name);
    break;
  case INT:
    doVarOp(8,name);
    sprintf(tmp, "prtai %s", name);
    break;
  case FLOAT:
    doVarOp(9,name);
    sprintf(tmp, "prtaf %s", name);
    break;
  case DOUBLE:
    doVarOp(10,name);
    sprintf(tmp, "prtad %s", name);
    break;
  case STRING:
    doVarOp(11,name);
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
    if(variable->type == STRING) {
      pushkiDirect(variable->stringSize);
      sprintf(tmp,"pushki %d",variable->stringSize);
      EmitLn(tmp);
      doOneByteOp(61);
      EmitLn("MUL");
    }
    doOneByteOp(32);
    EmitLn("popx");
    if(TOKEN != RIGHT_BRACKET) expected("closing array");
    next();
  }
  if(isArray) PrintArray(variable->type, variable->name);
  else PrintVar(variable->type, variable->name);
  doOneByteOp(1);
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
    doVarOp(38,name);
    sprintf(tmp, "rdc %s", name);
    break;
  case INT:
    doVarOp(39,name);
    sprintf(tmp, "rdi %s", name);
    break;
  case FLOAT:
    doVarOp(40,name);
    sprintf(tmp, "rdf %s", name);
    break;
  case DOUBLE:
    doVarOp(41,name);
    sprintf(tmp, "rdd %s", name);
    break;
  case STRING:
    doVarOp(42,name);
    sprintf(tmp, "rds %s", name);
  }
  EmitLn(tmp);
}

void ReadArray(char type, char *name) {
  switch(type) {
  case CHAR:
    doVarOp(43,name);
    sprintf(tmp, "rdac %s", name);
    break;
  case INT:
    doVarOp(44,name);
    sprintf(tmp, "rdai %s", name);
    break;
  case FLOAT:
    doVarOp(45,name);
    sprintf(tmp, "rdaf %s", name);
    break;
  case DOUBLE:
    doVarOp(46,name);
    sprintf(tmp, "rdad %s", name);
    break;
  case STRING:
    doVarOp(47,name);
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
    if(variable->type == STRING) {
      pushkiDirect(variable->stringSize);
      sprintf(tmp,"pushki %d",variable->stringSize);
      EmitLn(tmp);
      doOneByteOp(61);
      EmitLn("MUL");
    }
    doOneByteOp(32);
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
  pushki("0");
  EmitLn("pushki 0");
  doOneByteOp(64);
  EmitLn("cmp");
  if(TOKEN != RIGHT_PAREN) expected("closing paretheses");
  next();
  char l1[BUFFER_SIZE];
  genLabel(l1);
  doOneByteOp(49);
  doJump(l1);
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
  pushki("0");
  EmitLn("pushki 0");
  doOneByteOp(64);
  EmitLn("cmp");
  if(TOKEN != RIGHT_PAREN) expected("closing paretheses");
  next();
  char l1[BUFFER_SIZE];
  genLabel(l1);
  doOneByteOp(49);
  doJump(l1);
  sprintf(tmp, "jmpeq %s", l1);
  EmitLn(tmp);
  CodeBlock();
  doOneByteOp(48);
  doJump(l0);
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
  pushki("0");
  EmitLn("pushki 0");
  doOneByteOp(64);
  EmitLn("cmp");
  doOneByteOp(49);
  doJump(l3);
  sprintf(tmp, "jmpeq %s", l3);
  EmitLn(tmp);
  doOneByteOp(48);
  doJump(l2);
  sprintf(tmp, "jmp %s", l2);
  EmitLn(tmp);
  if(TOKEN != SEMI_COLON) expected("semicolon");
  next();
  EmitLabel(l1);
  if(TOKEN == NAME) DoAssignment();
  doOneByteOp(48);
  doJump(l0);
  sprintf(tmp, "jmp %s", l0);
  EmitLn(tmp);
  if(TOKEN != RIGHT_PAREN) expected("closing paretheses");
  next();
  EmitLabel(l2);
  CodeBlock();
  doOneByteOp(48);
  doJump(l1);
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

int main(int argc, char *argv[]) {
#ifdef _WIN32
  _setmode( _fileno(stdout), _O_BINARY);
#endif
  if(argc == 2)
    if(!strcmp(argv[1],"asm"))
      type_out = ASM;
    else
      type_out = BINARY;
  init();
  Language();
  resolvePendingLabels();
  if(type_out == BINARY)
    emitOutput();
  return 0;
}
