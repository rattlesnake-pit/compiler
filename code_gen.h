#ifndef CODE_GEN_H
#define CODE_GEN_H
struct symbol_row {
  char name[100];
  int start;
  char type;
  int stringSize;
};

void insertSymbol(char type, int type_sz, char* name);
void initBuffer();
void insertSymbol2(char type, int type_sz, char* name, int str_size);
struct symbol_row * findVariable(char *value);
void genLabel(char *label);
void EmitLabel(char *label);
void writeByte(unsigned char b);
void writeAddress(int a);
void doOneByteOp(unsigned char token);
void doVarOp(int token, char *value);
int findVariableAddress(char *value);
void writeInt(int value);
void writeFloat(float value);
void writeSize(char *value);
void doJump(char *label);
void doJumpFalse(char *label);
void writeString(char *value);
void pushkiDirect(int value);
void pushki(char *value);
void pushkf(char *value);
void pushks(char *value);
void addKint(char *value);
void addKfloat(char *value);
void addKstring(char *value);
void insertLabel(char *value);
void resolvePendingLabels();
void insertPendingLabel(char *label);
int findLabelAddress(char* label);
void emitOutput();
#endif
