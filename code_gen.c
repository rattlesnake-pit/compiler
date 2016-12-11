#include "string.h"
#include "code_gen.h"
#include "stdio.h"
#include "compiler.h"
#include "stdlib.h"

int PC = 162; // this address is where we start writing to memory
int DS = 13;
int ST_END = 0;
int LabelNumber = 0;
int st_end = 0;
int lt_end = 0;
int pending_end = 0;


struct symbol_row symbol_table[100];
/*
  anything after this is new and needs to be added to code_gen.h
*/


void initBuffer() {
  outBuffer[0] = 0x7f;
  outBuffer[1] = 0x45;
  outBuffer[2] = 0x4c;
  outBuffer[3] = 0x46;
  outBuffer[4] = 0x01;
  outBuffer[5] = 0x01;
  outBuffer[6] = 0x01;
  outBuffer[7] = 0x00;
  outBuffer[8] = 0x00;
  outBuffer[9] = 0x00;
  outBuffer[10] = 0x00;
  outBuffer[11] = 0x00;
  outBuffer[12] = 0x00;
  outBuffer[13] = 0x00;
  outBuffer[14] = 0x00;
  outBuffer[15] = 0x00;
  outBuffer[16] = 0x02;
  outBuffer[17] = 0x00;
  outBuffer[18] = 0x03;
  outBuffer[19] = 0x00;
  outBuffer[20] = 0x01;
  outBuffer[21] = 0x00;
  outBuffer[22] = 0x00;
  outBuffer[23] = 0x00;
  outBuffer[24] = 0xa2; // _start address
  outBuffer[25] = 0x80;
  outBuffer[26] = 0x04;
  outBuffer[27] = 0x08;
  outBuffer[28] = 0x34;
  outBuffer[29] = 0x00;
  outBuffer[30] = 0x00;
  outBuffer[31] = 0x00;
  outBuffer[32] = 0x00;
  outBuffer[33] = 0x00;
  outBuffer[34] = 0x00;
  outBuffer[35] = 0x00;
  outBuffer[36] = 0x00;
  outBuffer[37] = 0x00;
  outBuffer[38] = 0x00;
  outBuffer[39] = 0x00;
  outBuffer[40] = 0x34;
  outBuffer[41] = 0x00;
  outBuffer[42] = 0x20;
  outBuffer[43] = 0x00;
  outBuffer[44] = 0x02;
  outBuffer[45] = 0x00;
  outBuffer[46] = 0x00;
  outBuffer[47] = 0x00;
  outBuffer[48] = 0x00;
  outBuffer[49] = 0x00;
  outBuffer[50] = 0x00;
  outBuffer[51] = 0x00;
  outBuffer[52] = 0x01;
  outBuffer[53] = 0x00;
  outBuffer[54] = 0x00;
  outBuffer[55] = 0x00;
  outBuffer[56] = 0x00;
  outBuffer[57] = 0x00;
  outBuffer[58] = 0x00;
  outBuffer[59] = 0x00;
  outBuffer[60] = 0x00;
  outBuffer[61] = 0x80;
  outBuffer[62] = 0x04;
  outBuffer[63] = 0x08;
  outBuffer[64] = 0x00;
  outBuffer[65] = 0x80;
  outBuffer[66] = 0x04;
  outBuffer[67] = 0x08;
  outBuffer[68] = 0x9f; //ro memory size (will change)
  outBuffer[69] = 0x00;
  outBuffer[70] = 0x00;
  outBuffer[71] = 0x00;
  outBuffer[72] = 0x9f; //ro memory size(will change)
  outBuffer[73] = 0x00;
  outBuffer[74] = 0x00;
  outBuffer[75] = 0x00;
  outBuffer[76] = 0x05;
  outBuffer[77] = 0x00;
  outBuffer[78] = 0x00;
  outBuffer[79] = 0x00;
  outBuffer[80] = 0x00;
  outBuffer[81] = 0x10;
  outBuffer[82] = 0x00;
  outBuffer[83] = 0x00;
  outBuffer[84] = 0x01;
  outBuffer[85] = 0x00;
  outBuffer[86] = 0x00;
  outBuffer[87] = 0x00;
  outBuffer[88] = 0x00;
  outBuffer[89] = 0x00;
  outBuffer[90] = 0x00;
  outBuffer[91] = 0x00;
  outBuffer[92] = 0x00;
  outBuffer[93] = 0x90;
  outBuffer[94] = 0x04;
  outBuffer[95] = 0x08;
  outBuffer[96] = 0x00;
  outBuffer[97] = 0x90;
  outBuffer[98] = 0x04;
  outBuffer[99] = 0x08;
  outBuffer[100] = 0x00;
  outBuffer[101] = 0x00;
  outBuffer[102] = 0x00;
  outBuffer[103] = 0x00;
  outBuffer[104] = 0x12; // bss memory size(will change)
  outBuffer[105] = 0x00;
  outBuffer[106] = 0x00;
  outBuffer[107] = 0x00;
  outBuffer[108] = 0x06;
  outBuffer[109] = 0x00;
  outBuffer[110] = 0x00;
  outBuffer[111] = 0x00;
  outBuffer[112] = 0x00;
  outBuffer[113] = 0x10;
  outBuffer[114] = 0x00;
  outBuffer[115] = 0x00;
  //.text
  outBuffer[116] = 0x0a; // 10
  outBuffer[117] = 0x00;
  outBuffer[118] = 0x00;
  outBuffer[119] = 0x00;
  outBuffer[120] = 0x31;
  outBuffer[121] = 0xd2;
  outBuffer[122] = 0xf7;
  outBuffer[123] = 0x3d;
  outBuffer[124] = 0x74; //const 10 dir
  outBuffer[125] = 0x80;
  outBuffer[126] = 0x04;
  outBuffer[127] = 0x08;
  outBuffer[128] = 0x52;
  outBuffer[129] = 0x83;
  outBuffer[130] = 0xf8;
  outBuffer[131] = 0x00;
  outBuffer[132] = 0x74;
  outBuffer[133] = 0x05;
  outBuffer[134] = 0xe8;
  outBuffer[135] = 0xed;
  outBuffer[136] = 0xff;
  outBuffer[137] = 0xff;
  outBuffer[138] = 0xff;
  outBuffer[139] = 0x5a;
  outBuffer[140] = 0x83;
  outBuffer[141] = 0xc2;
  outBuffer[142] = 0x30;
  outBuffer[143] = 0x8b;
  outBuffer[144] = 0x1d;
  outBuffer[145] = 0x09; //memory address of ptr
  outBuffer[146] = 0x90;
  outBuffer[147] = 0x04;
  outBuffer[148] = 0x08;
  outBuffer[149] = 0x89;
  outBuffer[150] = 0x93;
  outBuffer[151] = 0x00; //memory address of buffer 
  outBuffer[152] = 0x90;
  outBuffer[153] = 0x04;
  outBuffer[154] = 0x08;
  outBuffer[155] = 0xfe;
  outBuffer[156] = 0x05;
  outBuffer[157] = 0x09;
  outBuffer[158] = 0x90;
  outBuffer[159] = 0x04;
  outBuffer[160] = 0x08;
  outBuffer[161] = 0xc3;

}
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
  for(int i = 0 ; i < 4; i++ ) {
    writeByte(value >> (i * 8));
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
    address += 0x8048000;
    int pc = pending_label_table[i].address;
    outBuffer[pc] = address;
    outBuffer[pc+1] = address >> 8;
    outBuffer[pc+2] = address >> 16;
    outBuffer[pc+3] = address >> 24;
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
    //mov dir
    writeByte(0xb8);

    if(address == NOT_FOUND) {
      insertPendingLabel(label);
      writeByte(0xFF);
      writeByte(0xFF);
      writeByte(0xFF);
      writeByte(0xFF);
    }
    else {
      writeInt(0x8048000 + address);
    }
    // jmp eax
    writeByte(0xff);
    writeByte(0xe0);
}

void doJumpFalse(char *label) {
    int address = findLabelAddress(label);
    //cmp eax, 0
    writeByte(0x83);
    writeByte(0xf8);
    writeByte(0x00);

    // jz +7
    writeByte(0x74);
    writeByte(0x02);
    // jmp short
    writeByte(0xeb);
    writeByte(0x07);
    //mov dir
    writeByte(0xb8);

    if(address == NOT_FOUND) {
      insertPendingLabel(label);
      writeByte(0xFF);
      writeByte(0xFF);
      writeByte(0xFF);
      writeByte(0xFF);
    }
    else {
      writeInt(0x8048000 + address);
    }
    // jmp eax
    writeByte(0xff);
    writeByte(0xe0);
}

void pushkiDirect(int value) {
    doOneByteOp(23);
    writeInt(value);
}

void pushki(char* value) {
    writeByte(0xb8);
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

void emitCode() {
  for(int i = 0; i < PC; i++) {
    printf("%c", outBuffer[i]);
  }
}

void exit_syscall() {
  writeByte(0x31);
  writeByte(0xc0);
  writeByte(0x40);
  writeByte(0x31);
  writeByte(0xdb);
  writeByte(0xcd);
  writeByte(0x80);
}
void emitOutput() {
  exit_syscall();
  outBuffer[68] = PC;
  outBuffer[72] = PC;
  outBuffer[104] = DS;
  emitCode();

}

//anything below this is old

void insertSymbol(char type, int type_sz, char* name) {
  struct symbol_row row;
  strcpy(row.name, name);
  row.start = DS;
  row.type = type;
  row.stringSize = type_sz;
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
  insertLabel(label);
  EmitLn(tmp);
}
