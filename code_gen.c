#include "string.h"
#include "code_gen.h"
#include "stdio.h"
#include "compiler.h"

int DS = 0;
int ST_END = 0;
int LabelNumber = 0;


struct symbol_row symbol_table[100];

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
