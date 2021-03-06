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

#include <llvm/Support/raw_ostream.h>

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
            llvm::StructType* listType;
            llvm::Value* generateGlovalVariable(
                const std::shared_ptr<variableAST>&);
            llvm::GlobalVariable* GeneratorGlobalString(
                std::shared_ptr<variableAST>);
            llvm::Value* generateVariable(const std::shared_ptr<variableAST>&,
                                          auto);
            llvm::AllocaInst* generateList(const std::shared_ptr<baseAST>&);
            llvm::AllocaInst* generateList(std::vector<llvm::Value*>);
            llvm::Value* generateifStmt(const std::shared_ptr<baseAST>&);
            llvm::Value* generateCond(const std::shared_ptr<baseAST>&);
            llvm::Value* generateString(std::shared_ptr<variableAST>);
            llvm::Instruction* generateCallFunc(const std::shared_ptr<baseAST>&);
            llvm::Instruction* generateLOp(const std::shared_ptr<funcCallAST>&);
            llvm::Instruction* generateReserved(const std::shared_ptr<funcCallAST>&);
            llvm::Instruction* generatemap(const std::shared_ptr<funcCallAST>&);
            llvm::Instruction* generateHighOrder(const std::shared_ptr<funcCallAST>&);
            llvm::Instruction* generateListGep(const std::shared_ptr<funcCallAST>&);
            llvm::CallInst* generatePrint(const std::shared_ptr<funcCallAST>&);
            llvm::Function* GeneratorFunction(std::shared_ptr<variableAST>);
            llvm::Function* GeneratorFunction(std::shared_ptr<baseAST>);
            llvm::Value* GeneratorStatement(std::shared_ptr<baseAST>);
            llvm::Value* GeneratorGlobalValue(
                const std::shared_ptr<baseAST>&,
                const llvm::ValueSymbolTable* vTable = nullptr);
        };
    }  // namespace codegen
}  // namespace otter

#endif
