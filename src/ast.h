#ifndef AST_H
#define AST_H

#include <llvm/IR/Value.h>
#include <memory>
#include <string>
#include <vector>

using std::string;
using std::unique_ptr;
using std::vector;

class ExprAST {
public:
  virtual ~ExprAST() = default;
  virtual llvm::Value *codegen() = 0;
};

class NumExprAST : public ExprAST {
  double val;

public:
  virtual ~NumExprAST() = default;
  NumExprAST(double val);
  llvm::Value *codegen() override;
};

class VarExprAST : public ExprAST {
  string name;

public:
  virtual ~VarExprAST() = default;
  VarExprAST(const string &name);
  llvm::Value *codegen() override;
};

class BinExprAST : public ExprAST {
  char op;
  unique_ptr<ExprAST> lhs, rhs;

public:
  virtual ~BinExprAST() = default;
  BinExprAST(char op, unique_ptr<ExprAST> lhs, unique_ptr<ExprAST> rhs);
  llvm::Value *codegen() override;
};

class CallExprAST : public ExprAST {
  string callee;
  vector<unique_ptr<ExprAST>> args;

public:
  virtual ~CallExprAST() = default;
  CallExprAST(const string &callee, vector<unique_ptr<ExprAST>> args);
  llvm::Value *codegen() override;
};

class PrototypeAST {
  string name;
  vector<string> args;

public:
  virtual ~PrototypeAST() = default;
  PrototypeAST(const string &name, vector<string> args);
  llvm::Value *codegen();
};

class FunctionAST {
  unique_ptr<PrototypeAST> proto;
  unique_ptr<ExprAST> body;

public:
  virtual ~FunctionAST() = default;
  FunctionAST(unique_ptr<PrototypeAST> proto, unique_ptr<ExprAST> body);
  llvm::Value *codegen();
};

std::unique_ptr<ExprAST> log_err(const char *str);

std::unique_ptr<PrototypeAST> log_err_p(const char *str);

llvm::Value *log_err_v(const char *Str);

#endif
