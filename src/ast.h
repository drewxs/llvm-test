#ifndef AST_H
#define AST_H

#include <memory>
#include <string>
#include <vector>

using namespace std;

class ExprAST;
class NumExprAST;
class VarExprAST;
class BinExprAST;
class CallExprAST;
class PrototypeAST;
class FunctionAST;

class ExprAST {
public:
  virtual ~ExprAST() = default;
};

class NumExprAST : public ExprAST {
  double val;

public:
  NumExprAST(double val);
};

class VarExprAST : public ExprAST {
  string name;

public:
  VarExprAST(const string &name);
};

class BinExprAST : public ExprAST {
  char op;
  unique_ptr<ExprAST> lhs, rhs;

public:
  BinExprAST(char op, unique_ptr<ExprAST> lhs, unique_ptr<ExprAST> rhs);
};

class CallExprAST : public ExprAST {
  string callee;
  vector<unique_ptr<ExprAST>> args;

public:
  CallExprAST(const string &callee, vector<unique_ptr<ExprAST>> args);
};

class PrototypeAST {
  string name;
  vector<string> args;

public:
  PrototypeAST(const string &name, vector<string> args);
};

class FunctionAST {
  unique_ptr<PrototypeAST> proto;
  unique_ptr<ExprAST> body;

public:
  FunctionAST(unique_ptr<PrototypeAST> proto, unique_ptr<ExprAST> body);
};

#endif
