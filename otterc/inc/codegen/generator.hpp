# ifndef _OTTER_CODEGEN_GENERATOR_HPP_
# define _OTTER_CODEGEN_GENERATOR_HPP_

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/GlobalVariable.h>
#include "../ast/ast.hpp"
#include "helper.hpp"

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
                llvm::GlobalVariable* generateGlovalVariable(std::shared_ptr<variableAST> var);
                llvm::GlobalVariable* GeneratorGlobalString(std::shared_ptr<variableAST> var);

        };
    } // namespace codegen
} //namespace otter

#endif
