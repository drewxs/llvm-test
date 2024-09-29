#include "constants.h"
#include "parser.h"

// static void init_mod() {
//   CONTEXT = std::make_unique<llvm::LLVMContext>();
//   MODULE = std::make_unique<llvm::Module>("JIT", *CONTEXT);
//   BUILDER = std::make_unique<llvm::IRBuilder<>>(*CONTEXT);
// }

int main() {
  fprintf(stderr, "kld> ");
  get_next_token();

  // init_mod();

  main_loop();

  MODULE->print(llvm::errs(), nullptr);

  return 0;
}
