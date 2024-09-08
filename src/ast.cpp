#include "ast.h"
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <utility>

NumExprAST::NumExprAST(double val) : val(val) {}

VarExprAST::VarExprAST(const string &name) : name(name) {}

BinExprAST::BinExprAST(char op, unique_ptr<ExprAST> lhs,
                       unique_ptr<ExprAST> rhs)
    : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

CallExprAST::CallExprAST(const string &callee, vector<unique_ptr<ExprAST>> args)
    : callee(callee), args(std::move(args)) {}

PrototypeAST::PrototypeAST(const string &name, vector<string> args)
    : name(name), args(std::move(args)) {}

FunctionAST::FunctionAST(unique_ptr<PrototypeAST> proto,
                         unique_ptr<ExprAST> body)
    : proto(std::move(proto)), body(std::move(body)) {}
