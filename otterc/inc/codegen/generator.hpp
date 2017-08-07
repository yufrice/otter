#ifndef _OTTER_CODEGEN_GENERATOR_HPP_
#define _OTTER_CODEGEN_GENERATOR_HPP_

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/ValueSymbolTable.h>
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
            llvm::BasicBlock* Entry;
            llvm::IRBuilder<>* Builder;
            llvm::Module* Module;
            llvm::Instruction* addModuleInst(llvm::Instruction*);
            llvm::Value* generateGlovalVariable(std::shared_ptr<variableAST>);
            llvm::GlobalVariable* GeneratorGlobalString(
                std::shared_ptr<variableAST>);
            llvm::Value* generateVariable(std::shared_ptr<variableAST>,auto);
            llvm::Value* generateString(std::shared_ptr<variableAST>);
            llvm::CallInst* generateCallFunc(const std::shared_ptr<baseAST>&, const llvm::ValueSymbolTable* vTable = new llvm::ValueSymbolTable());
            llvm::CallInst* generatePrint(const std::shared_ptr<baseAST>&, const llvm::ValueSymbolTable* vTable);
            llvm::Function* GeneratorFunction(std::shared_ptr<variableAST>);
            llvm::Value* GeneratorStatement(std::shared_ptr<baseAST>,
                                            llvm::Function*);
            llvm::Value* GeneratorGlobalValue(std::shared_ptr<baseAST>, TypeID,llvm::ValueSymbolTable* vTable = new llvm::ValueSymbolTable());
        };
    }  // namespace codegen
}  // namespace otter

#endif
