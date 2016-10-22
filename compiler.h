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
  PRINT,        // 8
  LEFT_PAREN,   // 9
  RIGHT_PAREN,  // 10
  ADD,          // 11
  SUB,          // 12
  MUL,          // 13
  DIV,          // 14
  LESS,         // 15
  LESS_EQUAL,   // 16
  GREATER,      // 17
  GREATER_EQUAL,// 18
  EQUAL,        // 19
  LEFT_BRACE,   // 20
  RIGHT_BRACE,  // 21
  LEFT_BRACKET, // 22
  RIGHT_BRACKET,// 23
  ASSIGN,       // 24
  SEMI_COLON,   // 25
  ENDLINE,      // 26
  COMMA,        // 27
  NAME,         // 28
  SYMBOL,       // 29
  NUMBER,       // 30
  DECIMAL       // 31
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


struct label_row {
  char name[BUFFER_SIZE];
  int address;
};

extern int DS; // symbol_def
extern int PC;
struct label_row label_table[100];
struct label_row pending_label_table[100];
extern int st_end;
extern int lt_end;
extern int pending_end;
#endif
