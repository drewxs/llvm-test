#include "ast.h"
#include "constants.h"
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <llvm/ADT/APFloat.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>
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

llvm::Value *BinExprAST::codegen() {
  llvm::Value *L = lhs->codegen();
  llvm::Value *R = rhs->codegen();

  if (!L || !R) {
    return nullptr;
  }

  switch (op) {
  case '+':
    return BUILDER->CreateFAdd(L, R, "addtmp");
  case '-':
    return BUILDER->CreateFSub(L, R, "subtmp");
  case '*':
    return BUILDER->CreateFMul(L, R, "multmp");
  case '/':
    return BUILDER->CreateFDiv(L, R, "divtmp");
  case '<':
    L = BUILDER->CreateFCmpULT(L, R, "cmptmp");
    return BUILDER->CreateUIToFP(L, llvm::Type::getDoubleTy(*CONTEXT),
                                 "booltmp");
  case '>':
    L = BUILDER->CreateFCmpUGT(L, R, "cmptmp");
    return BUILDER->CreateUIToFP(L, llvm::Type::getDoubleTy(*CONTEXT),
                                 "booltmp");
  default:
    return log_err_v("invalid binary operator");
  }
}

CallExprAST::CallExprAST(const string &callee, vector<unique_ptr<ExprAST>> args)
    : callee(callee), args(std::move(args)) {}

llvm::Value *CallExprAST::codegen() {
  llvm::Function *callee_f = MODULE->getFunction(callee);
  if (!callee_f) {
    return log_err_v("Unknown function referenced");
  }

  if (callee_f->arg_size() != args.size()) {
    return log_err_v("Incorrect # arguments passed");
  }

  vector<llvm::Value *> args_v;
  for (uint i = 0, e = args.size(); i != e; ++i) {
    args_v.push_back(args[i]->codegen());
    if (!args_v.back()) {
      return nullptr;
    }
  }

  return BUILDER->CreateCall(callee_f, args_v, "calltmp");
}

PrototypeAST::PrototypeAST(const string &name, vector<string> args)
    : name(name), args(std::move(args)) {}

string PrototypeAST::getName() { return name; }

llvm::Function *PrototypeAST::codegen() {
  vector<llvm::Type *> doubles(args.size(), llvm::Type::getDoubleTy(*CONTEXT));
  llvm::FunctionType *FT = llvm::FunctionType::get(
      llvm::Type::getDoubleTy(*CONTEXT), doubles, false);
  llvm::Function *F = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, name, MODULE.get());

  uint idx = 0;
  for (auto &arg : F->args()) {
    arg.setName(args[idx++]);
  }

  return F;
}

FunctionAST::FunctionAST(unique_ptr<PrototypeAST> proto,
                         unique_ptr<ExprAST> body)
    : proto(std::move(proto)), body(std::move(body)) {}

llvm::Function *FunctionAST::codegen() {
  llvm::Function *F = MODULE->getFunction(proto->getName());

  if (!F) {
    F = proto->codegen();
  }
  if (!F) {
    return nullptr;
  }

  if (!F->empty()) {
    return (llvm::Function *)log_err_v("Function already defined");
  }

  llvm::BasicBlock *BB = llvm::BasicBlock::Create(*CONTEXT, "entry", F);
  BUILDER->SetInsertPoint(BB);

  NAMED_VALUES.clear();
  for (auto &arg : F->args()) {
    NAMED_VALUES[string(arg.getName())] = &arg;
  }

  if (llvm::Value *ret_val = body->codegen()) {
    BUILDER->CreateRet(ret_val);
    verifyFunction(*F);
    return F;
  }

  F->eraseFromParent();
  return nullptr;
}

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
