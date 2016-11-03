#ifndef CODE_GEN_H
#define CODE_GEN_H
struct symbol_row {
  char name[100];
  int start;
  char type;
  int stringSize;
};

void insertSymbol(char type, int type_sz, char* name);
void insertSymbol2(char type, int type_sz, char* name, int str_size);
struct symbol_row * findVariable(char *value);
void genLabel(char *label);
void EmitLabel(char *label);
#endif
