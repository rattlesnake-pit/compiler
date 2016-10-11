#ifndef COMPILER_H
#define COMPILER_H

#define BUFFER_SIZE 256

#define NOT_FOUND -1

#define CHAR 'c'
#define INT 'i'
#define FLOAT 'f'
#define DOUBLE 'd'
#define STRING 's'

#define CHAR_SZ 1
#define INT_SZ 4
#define FLOAT_SZ 4
#define DOUBLE_SZ 8

enum TOKENS {
  CHAR_TYPE,
  INT_TYPE,
  FLOAT_TYPE,
  DOUBLE_TYPE,
  IF,
  ELSE,
  FOR,
  WHILE,
  LEFT_PAREN,
  RIGHT_PAREN,
  ADD,
  SUB,
  MUL,
  DIV,
  LESS,
  LESS_EQUAL,
  GREATER,
  GREATER_EQUAL,
  EQUAL,
  LEFT_BRACE,
  RIGHT_BRACE,
  LEFT_BRACKET,
  RIGHT_BRACKET,
  ASSIGN,
  SEMI_COLON,
  ENDLINE,
  NAME,
  SYMBOL,
  NUMBER,
  DECIMAL
};

extern char* str_tokens[];
extern int tokens_size;

void getChar();
int findAddress();
void next();
void nextString();
void init();
void scan();
void matchString();
void matchString2();
void error(char *s);
void expected(char* s);
int findVariableAddress(char* s);
void insertSymbol(char type, int type_sz, char* name);
int findLabelAddress(char* label);
void insertPendingLabel();
int isAlphaNum(char c);
int isDeclaration(enum TOKENS TOKEN);
int isAssignment(enum TOKENS TOKEN);
enum TOKENS TOKEN;
char look;
char VALUE[BUFFER_SIZE];
char tmp[BUFFER_SIZE];
char outBuffer [1000];

struct symbol_row {
  char name[BUFFER_SIZE];
  int start;
  char type;
};

struct label_row {
  char name[BUFFER_SIZE];
  int address;
};

extern int DS; // symbol_def
extern int PC;
struct symbol_row symbol_table[100];
struct label_row label_table[100];
struct label_row pending_label_table[100];
extern int st_end;
extern int lt_end;
extern int pending_end;
#endif
