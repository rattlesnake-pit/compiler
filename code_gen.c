#include "string.h"
#include "code_gen.h"
#include "stdio.h"
#include "compiler.h"
#include "stdlib.h"

int PC = 0;
int DS = 0;
int ST_END = 0;
int LabelNumber = 0;
int st_end = 0;
int lt_end = 0;
int pending_end = 0;


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

void resolvePendingLabels() {
  for(int i = 0; i < pending_end; i++) {
    int address = findLabelAddress(pending_label_table[i].name);
    if(address == NOT_FOUND) {
      sprintf(tmp, "LABEL '%s' NOT FOUND", pending_label_table[i].name);
      error(tmp);
    }
    int pc = pending_label_table[i].address;
    fprintf(stderr, "resolved label %s with pc: %d to address %d\n",
        pending_label_table[i].name, pc, address);
    outBuffer[pc] = address >> 8;
    outBuffer[pc+1] = address;
  }
}

void insertPendingLabel(char *label) {
  struct label_row row;
  row.address = PC;
  strcpy(row.name, label);
  pending_label_table[pending_end] = row;
  pending_end++;
}

int findLabelAddress(char* label) {
  for(int i = 0; i < lt_end; i++) {
    if(strcmp(label_table[i].name, label) == 0) {
      return label_table[i].address;
    }
  }
  return NOT_FOUND;
}

void doJump(char *label) {
    int address = findLabelAddress(label);
    if(address == NOT_FOUND) {
      insertPendingLabel(label);
      writeByte(0xFF);
      writeByte(0xFF);
    }
    else {
      writeByte(address>>8);
      writeByte(address);
    }
}

void pushkiDirect(int value) {
    doOneByteOp(23);
    writeInt(value);
}

void pushki(char* value) {
    doOneByteOp(23);
    addKint(value);
}

void pushkf(char* value) {
    doOneByteOp(24);
    addKfloat(value);
}

void pushks(char* value) {
    doOneByteOp(26);
    addKstring(value);
}

void insertLabel(char *value) {
  struct label_row row;
  strcpy(row.name, value);
  row.address = PC;
  label_table[lt_end] = row;
  lt_end++;
}

void emitId() {
  printf("(C)CHUNKUN");
}

void emitDS() {
  printf("%c%c",DS>>8,DS);
}

void emitCS() {
  printf("%c%c",PC>>8,PC);
}

void emitCode() {
  for(int i = 0; i < PC; i++) {
    printf("%c", outBuffer[i]);
  }
}

void emitHeader() {
  emitId();
  emitDS();
  emitCS();
}

void emitOutput() {
  emitHeader();
  emitCode();
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
  insertLabel(label);
}

void EmitLabel(char *label) {
  sprintf(tmp, "%s:", label);
  EmitLn(tmp);
}
