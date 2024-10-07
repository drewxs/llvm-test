#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include <map>
#include <memory>

extern std::map<char, int> binop_precedence;

int get_tok_precedence();

std::unique_ptr<ExprAST> parse_num_expr();
std::unique_ptr<ExprAST> parse_paren_expr();
std::unique_ptr<ExprAST> parse_ident_expr();
std::unique_ptr<ExprAST> parse_primary();
std::unique_ptr<ExprAST> parse_binop_rhs(int expr_prec,
                                         unique_ptr<ExprAST> lhs);
std::unique_ptr<ExprAST> parse_expr();
std::unique_ptr<PrototypeAST> parse_prototype();
std::unique_ptr<FunctionAST> parse_def();
std::unique_ptr<FunctionAST> parse_top_level_expr();
std::unique_ptr<PrototypeAST> parse_extern();

int get_next_token();

void main_loop();

#endif
