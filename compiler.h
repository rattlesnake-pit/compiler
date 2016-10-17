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
  CHAR_TYPE,    // 0
  INT_TYPE,     // 1
  FLOAT_TYPE,   // 2
  DOUBLE_TYPE,  // 3
  IF,           // 4
  ELSE,         // 5
  FOR,          // 6
  WHILE,        // 7
  LEFT_PAREN,   // 8
  RIGHT_PAREN,  // 9
  ADD,          // 10
  SUB,          // 11
  MUL,          // 12
  DIV,          // 13
  LESS,         // 14
  LESS_EQUAL,   // 15
  GREATER,      // 16
  GREATER_EQUAL,// 17
  EQUAL,        // 18
  LEFT_BRACE,   // 19
  RIGHT_BRACE,  // 20
  LEFT_BRACKET, // 21
  RIGHT_BRACKET,// 22
  ASSIGN,       // 23
  SEMI_COLON,   // 24
  ENDLINE,      // 25
  NAME,         // 26
  SYMBOL,       // 27
  NUMBER,       // 28
  DECIMAL       // 29
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
int isSpecificString(char *s);
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
