#include "parser.h"
#include "ast.h"
#include "globals.h"
#include "lexer.h"
#include "token.h"
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <llvm/Support/raw_ostream.h>
#include <map>
#include <memory>
#include <utility>

using std::map;

int cur_tok;
int get_next_token() { return cur_tok = gettok(); }

/// Holds the precedence for each defined binary operator.
map<char, int> binop_precedence = {
    {'<', 10},
    {'+', 20},
    {'-', 20},
    {'*', 40},
};

/// Get precedence of pending binary operator token.
int get_tok_precedence() {
  if (!isascii(cur_tok)) {
    return -1;
  }

  int tok_prec = binop_precedence[cur_tok];
  if (tok_prec <= 0) {
    return -1;
  }

  return tok_prec;
}

unique_ptr<ExprAST> parse_expr();

/// num_expr -> number
unique_ptr<ExprAST> parse_num_expr() {
  auto result = std::make_unique<NumExprAST>(num_val);
  get_next_token(); // consume number
  return result;
}

/// paren_expr -> '(' expr ')'
unique_ptr<ExprAST> parse_paren_expr() {
  get_next_token(); // consume '('
  auto result = parse_expr();
  if (!result) {
    return nullptr;
  }
  if (cur_tok != ')') {
    return log_err("expected ')'");
  }
  get_next_token(); // consume ')'
  return result;
}

/// ident_expr
///   -> ident
///   -> ident '(' expr ')'
unique_ptr<ExprAST> parse_ident_expr() {
  string id_name = ident_str;

  get_next_token(); // consume identifier

  if (cur_tok != '(') {
    return std::make_unique<VarExprAST>(id_name);
  }

  // Call
  get_next_token(); // consume '('
  vector<unique_ptr<ExprAST>> args;
  if (cur_tok != ')') {
    while (true) {
      if (auto arg = parse_expr()) {
        args.push_back(std::move(arg));
      } else {
        return nullptr;
      }

      if (cur_tok == ')') {
        break;
      }

      if (cur_tok != ',') {
        return log_err("expected ')'or ',' in argument list");
      }

      get_next_token();
    }
  }

  get_next_token(); // consume ')'

  return std::make_unique<CallExprAST>(id_name, std::move(args));
}

/// primary
///   -> ident_expr
///   -> num_expr
///   -> paren_expr
unique_ptr<ExprAST> parse_primary() {
  switch (cur_tok) {
  case Token::tok_ident:
    return parse_ident_expr();
  case Token::tok_num:
    return parse_num_expr();
  case '(':
    return parse_paren_expr();
  default:
    return log_err("unknown token when expecting an expression");
  }
}

/// binop_rhs -> ('+' primary)*
unique_ptr<ExprAST> parse_binop_rhs(int expr_prec, unique_ptr<ExprAST> lhs) {
  while (true) {
    int tok_prec = get_tok_precedence();

    if (tok_prec < expr_prec) {
      return lhs;
    }

    int binop = cur_tok;
    get_next_token();

    auto rhs = parse_primary();
    if (!rhs) {
      return nullptr;
    }

    int next_prec = get_tok_precedence();
    if (tok_prec < next_prec) {
      rhs = parse_binop_rhs(tok_prec + 1, std::move(rhs));
      if (!rhs) {
        return nullptr;
      }
    }

    lhs = std::make_unique<BinExprAST>(binop, std::move(lhs), std::move(rhs));
  }
}

/// expression -> primary binop_rhs
unique_ptr<ExprAST> parse_expr() {
  auto lhs = parse_primary();
  if (!lhs) {
    return nullptr;
  }

  return parse_binop_rhs(0, std::move(lhs));
}

/// prototype -> id '(' id* ')'
unique_ptr<PrototypeAST> parse_prototype() {
  if (cur_tok != tok_ident) {
    return log_err_p("Expected function name in prototype");
  }

  string fn_name = ident_str;
  get_next_token();

  if (cur_tok != '(') {
    return log_err_p("Expected '(' in prototype");
  }

  vector<string> arg_names;
  while (get_next_token() == tok_ident) {
    arg_names.push_back(ident_str);
  }

  if (cur_tok != ')') {
    return log_err_p("Expected ')' in prototype");
  }

  // success
  get_next_token(); // consume ')'

  return std::make_unique<PrototypeAST>(fn_name, std::move(arg_names));
}

/// def -> 'def' prototype expression
unique_ptr<FunctionAST> parse_def() {
  get_next_token(); // consume 'def'
  //
  auto proto = parse_prototype();
  if (!proto) {
    return nullptr;
  }
  if (auto e = parse_expr()) {
    return std::make_unique<FunctionAST>(std::move(proto), std::move(e));
  }

  return nullptr;
}

/// top_level_expr -> expression
unique_ptr<FunctionAST> parse_top_level_expr() {
  if (auto e = parse_expr()) {
    auto proto =
        std::make_unique<PrototypeAST>("__anon_expr", vector<string>());
    return std::make_unique<FunctionAST>(std::move(proto), std::move(e));
  }
  return nullptr;
}

/// external -> 'extern' prototype
unique_ptr<PrototypeAST> parse_extern() {
  get_next_token(); // consume 'extern'
  return parse_prototype();
}

/// Top-level parsing

void handle_def() {
  if (auto fn_ast = parse_def()) {
    if (auto *fn_IR = fn_ast->codegen()) {
      fprintf(stderr, "Read function definition:");
      fn_IR->print(llvm::errs());
      fprintf(stderr, "\n");
    }
  } else {
    get_next_token();
  }
}

void handle_extern() {
  if (auto proto_ast = parse_extern()) {
    if (auto *fn_IR = proto_ast->codegen()) {
      fprintf(stderr, "Read extern: ");
      fn_IR->print(llvm::errs());
      fprintf(stderr, "\n");
    }
  } else {
    get_next_token();
  }
}

void handle_top_level_expr() {
  if (auto fn_ast = parse_top_level_expr()) {
    if (auto *fn_IR = fn_ast->codegen()) {
      fprintf(stderr, "Read top-level expression:");
      fn_IR->print(llvm::errs());
      fprintf(stderr, "\n");

      fn_IR->eraseFromParent();
    }
  } else {
    get_next_token();
  }
}

void main_loop() {
  while (true) {
    fprintf(stderr, "kld> ");
    switch (cur_tok) {
    case Token::tok_def:
      handle_def();
      break;
    case Token::tok_extern:
      handle_extern();
      break;
    case Token::tok_eof:
      return;
    case ';': // ignore top-level semicolons
      get_next_token();
      break;
    default:
      handle_top_level_expr();
      break;
    }
  }
}
