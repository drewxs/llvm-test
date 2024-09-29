#include "ast.h"
#include "constants.h"
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <llvm/ADT/APFloat.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Value.h>
#include <utility>

NumExprAST::NumExprAST(double val) : val(val) {}

llvm::Value *NumExprAST::codegen() {
  return llvm::ConstantFP::get(*CONTEXT, llvm::APFloat(val));
}

VarExprAST::VarExprAST(const string &name) : name(name) {}

llvm::Value *VarExprAST::codegen() {
  llvm::Value *v = NAMED_VALUES[name];
  if (!v) {
    log_err_v("Unknown variable name");
  }
  return v;
}

BinExprAST::BinExprAST(char op, unique_ptr<ExprAST> lhs,
                       unique_ptr<ExprAST> rhs)
    : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

llvm::Value *BinExprAST::codegen() { return nullptr; }

CallExprAST::CallExprAST(const string &callee, vector<unique_ptr<ExprAST>> args)
    : callee(callee), args(std::move(args)) {}

llvm::Value *CallExprAST::codegen() { return nullptr; }

PrototypeAST::PrototypeAST(const string &name, vector<string> args)
    : name(name), args(std::move(args)) {}

llvm::Value *PrototypeAST::codegen() { return nullptr; }

FunctionAST::FunctionAST(unique_ptr<PrototypeAST> proto,
                         unique_ptr<ExprAST> body)
    : proto(std::move(proto)), body(std::move(body)) {}

llvm::Value *FunctionAST::codegen() { return nullptr; }

std::unique_ptr<ExprAST> log_err(const char *str) {
  fprintf(stderr, "Error: %s\n", str);
  return nullptr;
}

std::unique_ptr<PrototypeAST> log_err_p(const char *str) {
  log_err(str);
  return nullptr;
}

llvm::Value *log_err_v(const char *str) {
  log_err(str);
  return nullptr;
}
