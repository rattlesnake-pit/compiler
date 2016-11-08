#include "string.h"
#include "code_gen.h"
#include "stdio.h"
#include "compiler.h"

int DS = 0;
int ST_END = 0;
int LabelNumber = 0;


struct symbol_row symbol_table[100];
/*
  anything after this is new and needs to be added to code_gen.h
*/

void writeByte(unsigned char b) {
  outBuffer2[PC] = b;
  //fprintf(stderr, "wrote byte hex:%x dec:%d\n", b, b);
  PC++;//not sure if this should still be here
}

void writeAddress(int a) {
    writeByte(a>>8);
    writeByte(a);
    fprintf(stderr, "wrote 2 bytes\n");
}

void doOneByteOp(unsigned char token) {
  writeByte(token);
  next();
}

int findVariableAddress(char *value){
  for(int i = 0; i < st_end; i++){
    if(strcmp(symbol_table[i].name,value) == 0){
      return symbol_table[i].start;
    }
  }
  return -1;
}

void doVarOp(int token, char *value){
    doOneByteOp();
    int address = findVariableAddress(value);
    if(address != -1) {
        writeAddress(address);
    }
    else{
        error("WE FUCKED UP THERE'S NO ADDRESSS");
    }
}

void writeInt(int val){
    for(int i = 24; i >= 0; i -= 8){
        writeByte(val >> i);
    }
}

void addKint(int token){
    if(token != NUMBER){
        expected("Number");
    }
    int intValue = atoi(value);
    writeInt(intValue);
}

void writeFloat(float val){
    int x = *(int*)&val;
    writeInt(x);
}

void addKfloat(int token){
    if(token != NUMBER && token != DECIMAL){
        expected("Number or Decimal");
    }
    float floatValue = (float)atof(value);
    writeFloat(floatValue);
}

void writeSize(char *val){
    int size = strlen(val);
    unsigned char len = strlen(val);
    writeByte(len);
}

void writeString(char *val){
    for(size_t i = 0; i < strlen(val); i++){
        writeByte(val[i]);
    }
}

void addKstring(char *value){
    writeSize(value);
    writeString(value);
}

//anything below this is old

void insertSymbol(char type, int type_sz, char* name) {
  struct symbol_row row;
  strcpy(row.name, name);
  row.start = DS;
  row.type = type;
  symbol_table[ST_END] = row;
  ST_END++;
  DS += type_sz;
}

void insertSymbol2(char type, int type_sz, char* name, int str_size) {
  struct symbol_row row;
  strcpy(row.name, name);
  row.start = DS;
  row.type = type;
  row.stringSize = str_size;
  symbol_table[ST_END] = row;
  ST_END++;
  DS += type_sz;
}

struct symbol_row * findVariable(char *value){
  for(int i = 0; i < ST_END; i++){
    if(strcmp(symbol_table[i].name,value) == 0){
      return &symbol_table[i];
    }
  }
  return NULL;
}

void genLabel(char *label) {
  sprintf(label, "L%d", LabelNumber++);
}

void EmitLabel(char *label) {
  sprintf(tmp, "%s:", label);
  EmitLn(tmp);
}
