#include "stdio.h"
#include "compiler.h"
#include "string.h"
#include "strings.h"
#include "stdlib.h"
#include "code_gen.h"

char* str_tokens[] = {
  "char",
  "int",
  "float",
  "double",
  "string",
  "if",
  "else",
  "for",
  "while",
  "print",
  "read",
  "(",
  ")",
  "+",
  "-",
  "*",
  "/",
  "<",
  "<=",
  ">",
  ">=",
  "==",
  "!=",
  "{",
  "}",
  "[",
  "]",
  "=",
  ";",
  "\n",
  ",",
  "&&",
  "||",
  "!",
  "\"",
};

int tokens_size = sizeof str_tokens / sizeof *str_tokens;

void scan() {
  for(int i = 0; i < tokens_size; i++) {
    if(strcasecmp(VALUE, str_tokens[i]) == 0) {
      TOKEN = (enum TOKENS)i;
    }
  }
}

void getChar() {
  look = getchar();
}

char upcase(char c) {
  if(c>= 'a' && c<= 'z') {
    return ~(1<<5) & c;
  }
  return c;
}

void matchString(char* s) {
  if(strcmp(s, VALUE) != 0) {
    expected(s);
  }
}

int isSpecificString(char *s){
  return strcmp(s,VALUE) == 0;
}

void matchString2(char* s) {
  if(strcmp(s, VALUE) != 0) {
      expected(s);
  }
  nextString();
}

int isAlpha(char c) {
  return upcase(c) >= 'A' && upcase(c) <= 'Z';
}

int isNum(char c) {
  return c >= '0' && c <='9';
}

int isAlphaNum(char c) {
  return isAlpha(c) || isNum(c);
}

int isString(char c) {
    return c != '\0' && c != '\"' && c != '\'';
}

void getName() {
  int i = 0;
  while(isAlphaNum(look)) {
    VALUE[i] = look;
    i++;
    getChar();
  }
  VALUE[i] = '\0';
  TOKEN = NAME;
}

void getStringName() {
    int i = 0;
    while(isString(look)) {
        VALUE[i] =look;
        i++;
        getChar();
    }
    VALUE[i] = '\0';
    TOKEN = NAME;
}

void getNum() {
    int i = 0;
    int hasPoint = 0;
    if(look == '-') {
      VALUE[0] = '-';
      i++;
      getChar();
    }
    if(look == '.') {
      VALUE[i] = '0';
      VALUE[i+1] = '.';
      hasPoint = 1;
      i += 2;
      getChar();
    }
    while(isNum(look) || look == '.'){
        if(look == '.') {
          if(hasPoint) {
            error("A number cannot have more than one decimal point");
          }
          hasPoint = 1;
        }
        VALUE[i] = look;
        i++;
        getChar();
    }
    if(VALUE[i-1] == '.') {
      error("A number cannot end in a decimal point");
    }
    VALUE[i] = '\0';
    TOKEN = hasPoint? DECIMAL: NUMBER;
}

void handleWhite() {
  while(look == '\n' || look == ' ' || look == '\r' || look == '\t') {
	  getChar();
  }
}
void next() {
  handleWhite();
  if(isAlpha(look)) {
    getName();

  }
  else if(isNum(look) || look == '.') {
    getNum();
  }
  else {
    VALUE[0] = look;
    char prev = look;
    getChar();
    if((prev == '<' || prev == '>' || prev == '=' || prev == '!') && look == '=') {
      VALUE[1] = '=';
      VALUE[2] = '\0';
      getChar();
    }
    else if(prev == '&' && look == '&') {
      VALUE[1] = '&';
      VALUE[2] = '\0';
      getChar();
    }
    else if(prev == '|' && look == '|') {
      VALUE[1] = '|';
      VALUE[2] = '\0';
      getChar();
    }
    else VALUE[1] = '\0';
    TOKEN = SYMBOL;
  }
  scan();

}

void nextString() {
    getStringName();
    scan();
}

void init() {
  initBuffer();
  getChar();
}

void println(char* s) {
  fprintf(stderr, "%s\n", s);
}

void error(char* s) {
  println(s);
  exit(1);
}

void expected(char* s) {
  if(*s == '\n') s = "endline";
  sprintf(tmp, "EXPECTED '%s'" , s);
  error(tmp);
}
