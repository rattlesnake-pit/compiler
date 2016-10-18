#ifndef CODE_GEN_H
#define CODE_GEN_H
struct symbol_row {
  char name[100];
  int start;
  char type;
};

void insertSymbol(char type, int type_sz, char* name);
struct symbol_row * findVariable(char *value);
#endif
