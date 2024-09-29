#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <map>
#include <memory>
#include <string>

extern std::unique_ptr<llvm::LLVMContext> CONTEXT;
extern std::unique_ptr<llvm::IRBuilder<>> BUILDER;
extern std::unique_ptr<llvm::Module> MODULE;
extern std::map<std::string, llvm::Value *> NAMED_VALUES;

#endif
