#include "string.h"
#include "code_gen.h"
#include "stdio.h"
#include "compiler.h"
#include "stdlib.h"

int PC = 0;
int DS = 0;
int ST_END = 0;
int LabelNumber = 0;


struct symbol_row symbol_table[100];
/*
  anything after this is new and needs to be added to code_gen.h
*/

void writeByte(unsigned char b) {
  outBuffer[PC] = b;
  PC++;//not sure if this should still be here
}

void writeAddress(int a) {
    writeByte(a>>8);
    writeByte(a);
}

void doOneByteOp(unsigned char token) {
  writeByte(token);
  next();
}

int findVariableAddress(char *value){
  for(int i = 0; i < ST_END; i++){
    if(strcmp(symbol_table[i].name,value) == 0){
      return symbol_table[i].start;
    }
  }
  return -1;
}

void doVarOp(int token, char *value){
    doOneByteOp(token);
    int address = findVariableAddress(value);
    if(address != -1) {
        writeAddress(address);
    }
    else{
        error("WE FUCKED UP THERE'S NO ADDRESSS");
    }
}

void writeInt(int value){
    for(int i = 24; i >= 0; i -= 8){
        writeByte(value >> i);
    }
}

void addKint(char *value){
    int intValue = atoi(value);
    writeInt(intValue);
}

void writeFloat(float value){
    int x = *(int*)&value;
    writeInt(x);
}

void addKfloat(char *value){
    float floatValue = (float)atof(value);
    writeFloat(floatValue);
}

void writeSize(char *value){
    int size = strlen(value);
    unsigned char len = strlen(value);
    writeByte(len);
}

void writeString(char *value){
    for(size_t i = 0; i < strlen(value); i++){
        writeByte(value[i]);
    }
}

void addKstring(char *value){
    writeSize(value);
    writeString(value);
}

void doJump(char *label) {
    //TODO: implement dojump
}

void pushkiDirect(int value) {
    doOneByteOp(23);//replace with value of pushki
    writeInt(value);
}

void pushki(char* value) {
    doOneByteOp(23);//replace with value of pushki
    addKint(value);
}

void pushkf(char* value) {
    doOneByteOp(24);//replace with value of pushkf
    addKfloat(value);
}

void pushks(char* value) {
    doOneByteOp(26);//replace with value of pushks
    addKstring(value);
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
