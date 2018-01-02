#ifndef _OTTER_CODEGEN_GENERATOR_HPP_
#define _OTTER_CODEGEN_GENERATOR_HPP_

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/ValueSymbolTable.h>
#include <llvm/IR/Constants.h>
#include <ast/ast.hpp>
#include <logger/logger.hpp>
#include <codegen/context.hpp>
#include <codegen/helper.hpp>

namespace otter {
    namespace codegen {
        using namespace otter::ast;

        class Generator {
           public:
            Generator();
            ~Generator();
            std::unique_ptr<llvm::Module> generatorModule(
                const std::shared_ptr<moduleAST>& mod);

           private:
            logger::Logger logger;
            Context context;
            llvm::BasicBlock* Entry;
            llvm::IRBuilder<>* Builder;
            std::unique_ptr<llvm::Module> Module;
            llvm::Instruction* addModuleInst(llvm::Instruction*,
                                             bool flag = false);
            llvm::Value* generateGlovalVariable(
                const std::shared_ptr<variableAST>&);
            llvm::GlobalVariable* GeneratorGlobalString(
                std::shared_ptr<variableAST>);
            llvm::Value* generateVariable(const std::shared_ptr<variableAST>&,
                                          auto);
            llvm::AllocaInst* generateList(const std::shared_ptr<baseAST>&);
            llvm::Value* generateifStmt(const std::shared_ptr<baseAST>&);
            llvm::Value* generateCond(const std::shared_ptr<baseAST>&);
            llvm::Value* generateString(std::shared_ptr<variableAST>);
            llvm::Instruction* generateCallFunc(
                const std::shared_ptr<baseAST>&);
            llvm::Instruction* generateLOp(const std::shared_ptr<baseAST>&);
            llvm::CallInst* generatePrint(const std::shared_ptr<baseAST>&);
            llvm::Function* GeneratorFunction(std::shared_ptr<variableAST>);
            llvm::Value* GeneratorStatement(std::shared_ptr<baseAST>);
            llvm::Value* GeneratorGlobalValue(
                const std::shared_ptr<baseAST>&,
                const llvm::ValueSymbolTable* vTable = nullptr);
        };
    }  // namespace codegen
}  // namespace otter

#endif
