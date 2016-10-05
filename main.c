#include "compiler.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

int PC = 0;
int DS = 0;
int st_end = 0;
int lt_end = 0;
int pending_end = 0;


int readTokens() {
	while(look != EOF) {
		next();
	}
}

int main() {
#ifdef _WIN32
  _setmode( _fileno(stdout), _O_BINARY);
#endif
  init();
  readTokens();
  return 0;
}
