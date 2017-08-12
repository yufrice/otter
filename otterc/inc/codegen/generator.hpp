#ifndef _OTTER_CODEGEN_GENERATOR_HPP_
#define _OTTER_CODEGEN_GENERATOR_HPP_

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/ValueSymbolTable.h>
#include <ast/ast.hpp>
#include <codegen/context.hpp>
#include <codegen/helper.hpp>

namespace otter {
    namespace codegen {
        using namespace otter::ast;

        class Generator {
           public:
            Generator();
            ~Generator();
            llvm::Module* generatorModule(const std::shared_ptr<moduleAST> &mod);

           private:
            Context context;
            llvm::BasicBlock* Entry;
            llvm::IRBuilder<>* Builder;
            llvm::Module* Module;
            llvm::Instruction* addModuleInst(llvm::Instruction*,bool flag = false);
            llvm::Value* generateGlovalVariable(std::shared_ptr<variableAST>);
            llvm::GlobalVariable* GeneratorGlobalString(
                std::shared_ptr<variableAST>);
            llvm::Value* generateVariable(std::shared_ptr<variableAST>,auto);
            llvm::Value* generateString(std::shared_ptr<variableAST>);
            llvm::CallInst* generateCallFunc(const std::shared_ptr<baseAST>&, const llvm::Function* = nullptr);
            llvm::CallInst* generatePrint(const std::shared_ptr<baseAST>&, const llvm::Function*);
            llvm::Function* GeneratorFunction(std::shared_ptr<variableAST>);
            llvm::Value* GeneratorStatement(std::shared_ptr<baseAST>,
                                            const llvm::Function*);
            llvm::Value* GeneratorGlobalValue(const std::shared_ptr<baseAST>&, TypeID,const llvm::ValueSymbolTable* vTable = new llvm::ValueSymbolTable());
        };
    }  // namespace codegen
}  // namespace otter

#endif
