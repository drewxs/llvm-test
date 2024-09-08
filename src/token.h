#ifndef TOKEN_H
#define TOKEN_H

enum Token {
  tok_eof = -1,
  tok_def = -2,
  tok_extern = -3,
  tok_ident = -4,
  tok_num = -5,
};

#endif
