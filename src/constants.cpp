#include "constants.h"

std::unique_ptr<llvm::LLVMContext> THE_CONTEXT =
    std::make_unique<llvm::LLVMContext>();
std::unique_ptr<llvm::IRBuilder<>> BUILDER =
    std::make_unique<llvm::IRBuilder<>>(*THE_CONTEXT);
std::unique_ptr<llvm::Module> THE_MODULE =
    std::make_unique<llvm::Module>("my module", *THE_CONTEXT);
std::map<std::string, llvm::Value *> NAMED_VALUES;
