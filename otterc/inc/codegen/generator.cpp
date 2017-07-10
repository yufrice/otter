#include "generator.hpp"

namespace otter {
namespace codegen {
using namespace otter::ast;

llvm::LLVMContext Generator::TheContext;

Generator::Generator()
    : Builder(new llvm::IRBuilder<>(TheContext)),
      Module(new llvm::Module("OtterModule", TheContext)){};

Generator::~Generator() {
    delete this->Builder;
    delete this->Module;
}

llvm::Module* Generator::generatorModule(std::shared_ptr<moduleAST> mod) {
    return this->Module;
}
}  // namespace codegen
}  // namespace otter
