#include "lexer.h"
#include "globals.h"
#include "token.h"
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <string>

using std::string;

string ident_str;
double num_val;

int gettok() {
  static int last_char = ' ';

  while (isspace(last_char)) {
    last_char = getchar();
  }

  if (isalpha(last_char)) {
    ident_str = last_char;

    while (isalnum((last_char = getchar()))) {
      ident_str += last_char;
    }

    if (ident_str == "def") {
      return Token::tok_def;
    }
    if (ident_str == "extern") {
      return Token::tok_extern;
    }
    return Token::tok_ident;
  }

  if (isdigit(last_char) || last_char == '.') {
    string num_str;
    while (isdigit(last_char) || last_char == '.') {
      num_str += last_char;
      last_char = getchar();
    }

    num_val = strtod(num_str.c_str(), nullptr);
    return Token::tok_num;
  }

  if (last_char == '#') {
    while (last_char != EOF && last_char != '\n' && last_char != '\r') {
      last_char = getchar();
    }

    if (last_char != EOF) {
      return gettok();
    }
  }

  if (last_char == EOF) {
    return Token::tok_eof;
  }

  int this_char = last_char;
  last_char = getchar();
  return this_char;
}
