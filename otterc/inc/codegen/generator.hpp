# ifndef _OTTER_CODEGEN_GENERATOR_HPP_
# define _OTTER_CODEGEN_GENERATOR_HPP_

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/GlobalVariable.h>
#include "../ast/ast.hpp"

namespace otter {
    namespace codegen {
        using namespace otter::ast;

        class Generator {
            public:
                Generator();
                ~Generator();
                llvm::Module* generatorModule(std::shared_ptr<moduleAST> mod);

            private:
                static llvm::LLVMContext TheContext;
                llvm::IRBuilder<>* Builder;
                llvm::Module* Module;

        };
    } // namespace codegen
} //namespace otter

#endif
