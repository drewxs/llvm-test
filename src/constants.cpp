#include "constants.h"

std::unique_ptr<llvm::LLVMContext> CONTEXT =
    std::make_unique<llvm::LLVMContext>();
std::unique_ptr<llvm::IRBuilder<>> BUILDER =
    std::make_unique<llvm::IRBuilder<>>(*CONTEXT);
std::unique_ptr<llvm::Module> MODULE =
    std::make_unique<llvm::Module>("my module", *CONTEXT);
std::map<std::string, llvm::Value *> NAMED_VALUES;
