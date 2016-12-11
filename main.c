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
void printLn();
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
    {
      int address = findVariableAddress(name);
      writeByte(0xa1);
      writeInt(0x8049000 + address);
      sprintf(tmp, "pushi %s", name);
      break;
    }
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
    //imul ebx, 4
    writeByte(0x6b);
    writeByte(0xdb);
    writeByte(0x04);
    //mov eax, [ebx + dir]
    writeByte(0x8b);
    writeByte(0x83);
    writeInt(0x8049000 + findVariableAddress(name));
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


void DoLoadString(struct symbol_row * variable) {
  /*
   *  add ebx,0x8049000
   *  add ecx,0x8049000
   *  mov al,[ebx]
   *  mov [ecx],al
   *  cmp al,0x0
   *  jz 0x18
   *  inc ecx
   *  inc ebx
   *  jmp short 0xc
   *
   */
  writeByte(0x81);
  writeByte(0xc3);
  writeByte(0x00);
  writeByte(0x90);
  writeByte(0x04);
  writeByte(0x08);

  writeByte(0x81);
  writeByte(0xc1);
  writeByte(0x00);
  writeByte(0x90);
  writeByte(0x04);
  writeByte(0x08);

  writeByte(0x8a);
  writeByte(0x03);

  writeByte(0x88);
  writeByte(0x01);

  writeByte(0x3c);
  writeByte(0x00);

  writeByte(0x74);
  writeByte(0x04);

  writeByte(0x41);

  writeByte(0x43);

  writeByte(0xeb);
  writeByte(0xf4);
} 
void LoadString(char *left, char *right) {
  struct symbol_row * variable = findVariable(right);
  if(variable == NULL) error("Undefined variable");
  struct symbol_row * leftVar = findVariable(left);
  int isArray = 0;
  next();
  //xor ebx, ebx
  writeByte(0x31);
  writeByte(0xdb);
  if(TOKEN == LEFT_BRACKET) {
    isArray = 1;
    next();
    Expression();
    // mov ebx, eax
    writeByte(0x89);
    writeByte(0xc3);

    int stringSize = variable->stringSize;

    //imul ebx, stringsize
    writeByte(0x6b);
    writeByte(0xdb);
    writeByte(stringSize);
    if(TOKEN != RIGHT_BRACKET) expected("closing array bracket");
    next();
  }
  //add ebx, dir
  //add ecx, dir
  writeByte(0x83);
  writeByte(0xc3);
  writeByte(variable->start);
  writeByte(0x83);
  writeByte(0xc1);
  writeByte(leftVar->start);
  DoLoadString(variable);
  //if(isArray)LoadArray(variable->type, variable->name);
  //else LoadVar(variable->type, variable->name);
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
    // mov ebx, eax
    writeByte(0x89);
    writeByte(0xc3);
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
  writeByte(0x0f);
  writeByte(0xaf);
  writeByte(0x04);
  writeByte(0x24);
  writeByte(0x83);
  writeByte(0xc4);
  writeByte(0x04);
  EmitLn("mul");
}

void Divide() {
  next();
  Factor();
  writeByte(0x89);
  writeByte(0xc2);
  writeByte(0x58);
  writeByte(0x99);
  writeByte(0xf7);
  writeByte(0xfa);
  EmitLn("div");
}

void Push() {
  writeByte(0x50);
}

void Pop() {
  writeByte(0x83);
  writeByte(0xc4);
  writeByte(0x04);
}

void MulExpression() {
  Factor();
  while(TOKEN == MUL || TOKEN == DIV) {
    Push();
    switch(TOKEN) {
      case MUL: Multiply(); break;
      case DIV: Divide(); break;
    }
  }
}

void Add() {
  next();
  MulExpression();
  writeByte(0x03);
  writeByte(0x04);
  writeByte(0x24);
  writeByte(0x83);
  writeByte(0xc4);
  writeByte(0x04);
  EmitLn("add");
}

void Substract() {
  next();
  MulExpression();
  writeByte(0x2b);
  writeByte(0x04);
  writeByte(0x24);
  writeByte(0xf7);
  writeByte(0xd8);
  writeByte(0x83);
  writeByte(0xc4);
  writeByte(0x04);
  EmitLn("sub");
}

int isVarString(char *name) {
  struct symbol_row * variable = findVariable(name);
  if(variable == NULL) error("Undefined variable");
  if(variable->type == STRING)
    return 1;
  return 0;
}

void AssignConstant(char *name) {
  int sz = strlen(VALUE);
  if(sz > 256){expected("string is too long");}
  //pushks(temp);
  struct symbol_row * variable = findVariable(name);
  int address = variable->start;
  int totalSize = variable->stringSize;
  for(int i = 0; i < sz; i++) {
    //mov byte dword [ecx + dir], val
    writeByte(0xc6);
    writeByte(0x81);
    writeInt(0x8049000 + address + i);
    writeByte(VALUE[i]);
  }
  for(int i = sz; i < totalSize; i++) {
    writeByte(0xc6);
    writeByte(0x81);
    writeInt(0x8049000 + address + i);
    writeByte(0x00);
  }

  sprintf(tmp,"pushks \"%s\"", VALUE);
  EmitLn(tmp);
}

void stringExpression(char *name) {
  if(TOKEN == QUOTE) {
    nextString();
    AssignConstant(name);//this should do pushks
    next();
    matchString("\"");
    next();
  }
  else {
    if(TOKEN == NAME) {
      LoadString(name, VALUE);
    }
  }
}

void Expression() {
  MulExpression();
  while(TOKEN == ADD || TOKEN == SUB) {
    Push();
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


void Compare() {
  next();
  Expression();
  writeByte(0x3b);
  writeByte(0x04);
  writeByte(0x24);
  EmitLn("cmp");
}

void Equals() {
  Compare();
  writeByte(0x0f);
  writeByte(0x94);
  writeByte(0xc0);
  writeByte(0x0f);
  writeByte(0xbe);
  writeByte(0xc0);
}

void NotEqual() {
  Compare();
  writeByte(0x0f);
  writeByte(0x95);
  writeByte(0xc0);
  writeByte(0x0f);
  writeByte(0xbe);
  writeByte(0xc0);
}

void Less() {
  Compare();
  writeByte(0x0f);
  writeByte(0x9f);
  writeByte(0xc0);
  writeByte(0x0f);
  writeByte(0xbe);
  writeByte(0xc0);
  
}

void LessEqual() {
  Compare();
  writeByte(0x0f);
  writeByte(0x9d);
  writeByte(0xc0);
  writeByte(0x0f);
  writeByte(0xbe);
  writeByte(0xc0);
}

void Greater() {
  Compare();
  writeByte(0x0f);
  writeByte(0x9c);
  writeByte(0xc0);
  writeByte(0x0f);
  writeByte(0xbe);
  writeByte(0xc0);
}

void GreaterEqual() {
  Compare();
  writeByte(0x0f);
  writeByte(0x9e);
  writeByte(0xc0);
  writeByte(0x0f);
  writeByte(0xbe);
  writeByte(0xc0);
}

void Relation() {
  Expression();
  if(isRelop(TOKEN)) {
    Push();
    switch(TOKEN) {
      case EQUAL: Equals(); break;
      case LESS: Less(); break;
      case LESS_EQUAL: LessEqual(); break;
      case GREATER: Greater(); break;
      case GREATER_EQUAL: GreaterEqual(); break;
      case NOT_EQUAL: NotEqual(); break;
    }
    Pop();
  }
}

void NotIt() {
  writeByte(0xf7);
  writeByte(0xd0);
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
  writeByte(0x23);
  writeByte(0x04);
  writeByte(0x24);
  Pop();
  EmitLn("mul");
}

void BoolTerm() {
  NotFactor();
  while(TOKEN == AND) {
    Push();
    next();
    NotFactor();
    DoAnd();
  }
}

void DoOr() {
  writeByte(0x0b);
  writeByte(0x04);
  writeByte(0x24);
  Pop();
  EmitLn("add");
}

void BoolExpression() {
  BoolTerm();
  while(TOKEN == OR) {
    Push();
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
    writeByte(0xa3);
    writeInt(0x8049000 + variable->start);
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
    //mov ebx, ecx
    writeByte(0x89);
    writeByte(0xcb);
    //imul ebx, 4
    writeByte(0x6b);
    writeByte(0xdb);
    writeByte(0x04);
    // mov [ebx + dir], eax
    writeByte(0x89);
    writeByte(0x83);
    writeInt(0x8049000 + variable->start);
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
  //xor ecx, ecx
  writeByte(0x31);
  writeByte(0xc9);
  if(TOKEN == LEFT_BRACKET) {
    isArray = 1;
    next();
    Expression();
    // mov ecx, eax
    writeByte(0x89);
    writeByte(0xc1);

    if(isVarString(name)) {
      int strSize = obtainStringSize(name);
      //imul ecx, strsize
      writeByte(0x6b);
      writeByte(0xc9);
      writeByte(strSize);
    }
    if(TOKEN != RIGHT_BRACKET) expected("closing array");
    next();
  }
  matchString("=");
  next();
  if(isVarString(name)){
    stringExpression(name);
  }
  else {
    BoolExpression();
    if(isArray) {
      StoreArray(name);
    }
    else {
      StoreVar(name);
    }

  }
}

void PrintVar(char type, char *name) {
  switch(type) {
  case CHAR:
    sprintf(tmp, "prtc %s", name);
    doVarOp(2,name);
    break;
  case INT:{
    int address = findVariableAddress(name);
    //mov
    writeByte(0xa1);
    writeInt(0x8049000 + address);
    //call
    writeByte(0xbb);
    writeByte(0x78);
    writeByte(0x80);
    writeByte(0x04);
    writeByte(0x08);
    writeByte(0xff);
    writeByte(0xd3);
    //print
    writeByte(0xb0);
    writeByte(0x04);
    writeByte(0xb3);
    writeByte(0x01);
    writeByte(0xb9);
    writeByte(0x00);
    writeByte(0x90);
    writeByte(0x04);
    writeByte(0x08);
    writeByte(0x8a);
    writeByte(0x15);
    writeByte(0x09);
    writeByte(0x90);
    writeByte(0x04);
    writeByte(0x08);
    writeByte(0xcd);
    writeByte(0x80);
    // reset ptr
    writeByte(0xc7);
    writeByte(0x05);
    writeByte(0x09);
    writeByte(0x90);
    writeByte(0x04);
    writeByte(0x08);
    writeByte(0x00);
    writeByte(0x00);
    writeByte(0x00);
    writeByte(0x00);

    sprintf(tmp, "prti %s", name);}
    break;
  case FLOAT:
    doVarOp(4,name);
    sprintf(tmp, "prtf %s", name);
    break;
  case DOUBLE:
    doVarOp(5,name);
    sprintf(tmp, "prtd %s", name);
    break;
  case STRING: {
    //doVarOp(6,name);
    sprintf(tmp, "prts %s", name);
    struct symbol_row *variable = findVariable(name);
    //print
    writeByte(0xb0);
    writeByte(0x04);
    writeByte(0xb3);
    writeByte(0x01);
    writeByte(0xb9);
    writeInt(0x8049000 + variable->start);
    writeByte(0xb2);
    writeByte(variable->stringSize);
    writeByte(0xcd);
    writeByte(0x80);
   }
  }
  EmitLn(tmp);

}

void PrintArray(char type, char *name) {
  switch(type) {
  case CHAR:
    doVarOp(7,name);
    sprintf(tmp, "prtac %s", name);
    break;
  case INT: {
    // imul ebx, 4
    writeByte(0x6b);
    writeByte(0xdb);
    writeByte(0x04);
    //mov eax, [ebx + dir]
    writeByte(0x8b);
    writeByte(0x83);
    writeInt(0x8049000 + findVariableAddress(name));
    //call
    writeByte(0xbb);
    writeByte(0x78);
    writeByte(0x80);
    writeByte(0x04);
    writeByte(0x08);
    writeByte(0xff);
    writeByte(0xd3);
    //print
    writeByte(0xb0);
    writeByte(0x04);
    writeByte(0xb3);
    writeByte(0x01);
    writeByte(0xb9);
    writeByte(0x00);
    writeByte(0x90);
    writeByte(0x04);
    writeByte(0x08);
    writeByte(0x8a);
    writeByte(0x15);
    writeByte(0x09);
    writeByte(0x90);
    writeByte(0x04);
    writeByte(0x08);
    writeByte(0xcd);
    writeByte(0x80);
    // reset ptr
    writeByte(0xc7);
    writeByte(0x05);
    writeByte(0x09);
    writeByte(0x90);
    writeByte(0x04);
    writeByte(0x08);
    writeByte(0x00);
    writeByte(0x00);
    writeByte(0x00);
    writeByte(0x00);
    break;}
  case FLOAT:
    doVarOp(9,name);
    sprintf(tmp, "prtaf %s", name);
    break;
  case DOUBLE:
    doVarOp(10,name);
    sprintf(tmp, "prtad %s", name);
    break;
  case STRING:{
    sprintf(tmp, "prtas %s", name);
    struct symbol_row *variable = findVariable(name);
    /*
     * add ebx, 0x8049000
     * add ebx, variable start (only 8 bits)
     * mov ecx, ebx
     * mov al, 0x4
     * mov bl, 0x1
     * mov dl, stringSize
     * int 0x80
     */
    writeByte(0x81);
    writeByte(0xc3);
    writeByte(0x00);
    writeByte(0x90);
    writeByte(0x04);
    writeByte(0x08);

    writeByte(0x83);
    writeByte(0xc3);
    writeByte(variable->start);

    writeByte(0x89);
    writeByte(0xd9);

    writeByte(0xb0);
    writeByte(0x04);

    writeByte(0xbb);
    writeByte(0x01);
    writeByte(0x00);
    writeByte(0x00);
    writeByte(0x00);

    writeByte(0xb2);
    writeByte(variable->stringSize);
    writeByte(0xcd);
    writeByte(0x80);
    break; }
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
    // mov ebx, eax
    writeByte(0x89);
    writeByte(0xc3);
    if(variable->type == STRING) {
      //imul ebx, strsize
      writeByte(0x6b);
      writeByte(0xdb);
      writeByte(variable->stringSize);
    }
    if(TOKEN != RIGHT_BRACKET) expected("closing array");
    next();
  }
  if(isArray) PrintArray(variable->type, variable->name);
  else PrintVar(variable->type, variable->name);
  //doOneByteOp(1);
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
  if(TOKEN != RIGHT_PAREN) expected("closing paretheses");
  next();
  char l1[BUFFER_SIZE];
  char l2[BUFFER_SIZE];
  genLabel(l1);
  strcpy(l2,l1);
  doJumpFalse(l1);
  CodeBlock();
  if(TOKEN == ELSE) {
    next();
    genLabel(l2);
    doJump(l2);
    EmitLabel(l1);
    CodeBlock();
  }
  EmitLabel(l2);
}

void DoWhile() {
  char l0[BUFFER_SIZE];
  next();
  if(TOKEN != LEFT_PAREN) expected("opening paretheses");
  next();
  genLabel(l0);
  EmitLabel(l0);
  BoolExpression();
  if(TOKEN != RIGHT_PAREN) expected("closing paretheses");
  next();
  char l1[BUFFER_SIZE];
  genLabel(l1);
  doJumpFalse(l1);
  CodeBlock();
  doJump(l0);
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
  doJumpFalse(l3);
  doJump(l2);
  if(TOKEN != SEMI_COLON) expected("semicolon");
  next();
  EmitLabel(l1);
  if(TOKEN == NAME) DoAssignment();
  doJump(l0);
  if(TOKEN != RIGHT_PAREN) expected("closing paretheses");
  next();
  EmitLabel(l2);
  CodeBlock();
  doJump(l1);
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
    printLn();
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

void printLn() {
/*
 * mov byte [dword 0x8049000],0xa
 * mov al,0x4
 * xor ebx,ebx
 * inc ebx
 * mov ecx,0x8049000
 * xor edx,edx
 * inc edx
 * int 0x80
 */
  writeByte(0xc6);
  writeByte(0x05);
  writeByte(0x00);
  writeByte(0x90);
  writeByte(0x04);
  writeByte(0x08);
  writeByte(0x0a);
  writeByte(0xb0);
  writeByte(0x04);
  writeByte(0x31);
  writeByte(0xdb);
  writeByte(0x43);
  writeByte(0xb9);
  writeByte(0x00);
  writeByte(0x90);
  writeByte(0x04);
  writeByte(0x08);
  writeByte(0x31);
  writeByte(0xd2);
  writeByte(0x42);
  writeByte(0xcd);
  writeByte(0x80);
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
  if( argc == 2 && !strcmp(argv[1],"asm"))
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
