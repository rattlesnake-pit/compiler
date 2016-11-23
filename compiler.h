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

#define ASM 10
#define BINARY 11

int type_out;

enum TOKENS {
  CHAR_TYPE,    // 0
  INT_TYPE,     // 1
  FLOAT_TYPE,   // 2
  DOUBLE_TYPE,  // 3
  STRING_TYPE,  // 4
  IF,           // 5
  ELSE,         // 6
  FOR,          // 7
  WHILE,        // 8
  PRINT,        // 9
  READ,         //10
  LEFT_PAREN,   // 11
  RIGHT_PAREN,  // 12
  ADD,          // 13
  SUB,          // 14
  MUL,          // 15
  DIV,          // 16
  LESS,         // 17
  LESS_EQUAL,   // 18
  GREATER,      // 19
  GREATER_EQUAL,// 20
  EQUAL,        // 21
  NOT_EQUAL,    // 22
  LEFT_BRACE,   // 23
  RIGHT_BRACE,  // 24
  LEFT_BRACKET, // 25
  RIGHT_BRACKET,// 26
  ASSIGN,       // 27
  SEMI_COLON,   // 28
  ENDLINE,      // 29
  COMMA,        // 30
  AND,          // 30
  OR,           // 31
  NOT,          // 32
  QUOTE,        // 33
  NAME,         // 34
  SYMBOL,       // 35
  NUMBER,       // 36
  DECIMAL       // 37
};

extern char* str_tokens[];
extern int tokens_size;

void EmitLn(char * str);
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
int isRelop(enum TOKENS TOKEN);
int getStringSize();
enum TOKENS TOKEN;
char look;
char VALUE[BUFFER_SIZE];
char tmp[BUFFER_SIZE];
char outBuffer[1000];


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
