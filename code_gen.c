#include "string.h"
#include "code_gen.h"

int DS = 0;
int ST_END = 0;


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

struct symbol_row * findVariable(char *value){
  for(int i = 0; i < ST_END; i++){
    if(strcmp(symbol_table[i].name,value) == 0){
      return &symbol_table[i];
    }
  }
  return NULL;
}
