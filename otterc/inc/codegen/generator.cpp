#include "generator.hpp"

namespace otter {
    namespace codegen {

        using namespace otter::ast;
        using namespace llvm;

        LLVMContext Generator::TheContext;

        Generator::Generator()
            : Builder(new IRBuilder<>(TheContext)),
              Module(new llvm::Module("OtterModule", TheContext)){};

        Generator::~Generator() {
            delete this->Builder;
            delete this->Module;
        }

        Module* Generator::generatorModule(std::shared_ptr<moduleAST> mod) {
            FunctionType* funcType = FunctionType::get(
                llvm::Type::getInt32Ty(this->TheContext), false);
            Function* mainFunc = Function::Create(
                funcType, Function::ExternalLinkage, "main", this->Module);

            BasicBlock* entry =
                BasicBlock::Create(this->TheContext, "entrypoint", mainFunc);
            Builder->SetInsertPoint(entry);

            for (auto var : mod->Vars) {
                if (!generateGlovalVariable(var)) {
                    // throw std::string("type error");
                }
            }

            Builder->CreateRetVoid();
            Module->dump();
            return this->Module;
        }

        bool Generator::generateGlovalVariable(
            std::shared_ptr<variableAST> var) {
            if (detail::sharedIsa<functionAST>(var->Val)) {
                if (this->GeneratorFunction(var, var->Type)) {
                    return true;
                }
            } else if (detail::sharedIsa<stringAST>(var->Val)) {
                if (TypeID::String == var->Type) {
                    if (this->GeneratorGlobalString(var)) {
                        return true;
                    }
                }
            } else if (detail::sharedIsa<identifierAST>(var->Val)) {
                std::cout << "id" << std::endl;
            } else {
                Type* valueType;
                if (var->Type == TypeID::Int) {
                    valueType = Type::getInt32Ty(this->TheContext);
                } else {
                    valueType = Type::getDoubleTy(this->TheContext);
                }
                Value* value = this->GeneratorValue(var->Val, valueType);
                if (value) {
                    auto gvar = new GlobalVariable(
                        *this->Module, valueType, false,
                        GlobalVariable::LinkageTypes::CommonLinkage, nullptr,
                        var->Name);
                    // gvar->setInitializer(value);
                    this->Builder->CreateStore(gvar, value);
                    if (gvar) {
                        return true;
                    }
                } else {
                    return false;
                }
            }
        }

        GlobalVariable* Generator::GeneratorGlobalString(
            std::shared_ptr<variableAST> var) {
            if (auto rawStr = detail::sharedCast<stringAST>(var->Val)) {
                ArrayType* Type =
                    ArrayType::get(IntegerType::get(this->TheContext, 8),
                                   (rawStr->Str).length() + 2);
                auto Name = var->Name;
                GlobalVariable* gvar =
                    new GlobalVariable(*this->Module, Type, true,
                                       GlobalValue::CommonLinkage, 0, Name);

                Constant* strCons =
                    ConstantDataArray::getString(this->TheContext, rawStr->Str);
                // llvm::Value* helloWorld =

                gvar->setInitializer(strCons);
                return gvar;
            }
        }

        Function* Generator::GeneratorFunction(std::shared_ptr<variableAST> var,
                                               TypeID funcType) {
            FunctionType* Type;
            std::vector<llvm::Type*> typeArgs;
            if (auto rawVal = detail::sharedCast<functionAST>(var->Val)) {
                for (auto argType : rawVal->Types) {
                    if (argType == TypeID::Int) {
                        typeArgs.emplace_back(
                            Type::getInt32Ty(this->TheContext));
                    } else if (argType == TypeID::Double) {
                        typeArgs.emplace_back(
                            Type::getDoubleTy(this->TheContext));
                    } else if (argType == TypeID::String) {
                        typeArgs.emplace_back(
                            Type::getInt32Ty(this->TheContext));
                    }
                }
            }
            ArrayRef<llvm::Type*> argsRef(typeArgs);
            if (funcType == TypeID::Int) {
                Type = FunctionType::get(
                    llvm::Type::getInt32Ty(this->TheContext), argsRef, false);
            } else if (funcType == TypeID::Double) {
                Type = FunctionType::get(
                    llvm::Type::getDoubleTy(this->TheContext), argsRef, false);
            } else if (funcType == TypeID::String) {
                Type = FunctionType::get(
                    llvm::Type::getInt32Ty(this->TheContext), argsRef, false);
            } else if (funcType == TypeID::Unit) {
                Type = FunctionType::get(
                    llvm::Type::getVoidTy(this->TheContext), argsRef, false);
            }

            Function::Create(Type, llvm::Function::ExternalLinkage, var->Name,
                             this->Module);
        }

        Value* Generator::GeneratorValue(std::shared_ptr<baseAST> var,
                                         Type* valueType) {
            if (auto rawVal = detail::sharedCast<identifierAST>(var)) {
                return this->Builder->CreateLoad(
                    this->Module->getGlobalVariable(rawVal->Ident), "");
            } else if (auto rawVal = detail::sharedCast<numberAST>(var)) {
                return ConstantInt::getSigned(valueType, rawVal->Val);
            } else if (auto rawVal = detail::sharedCast<binaryExprAST>(var)) {
                if (rawVal->Rhs) {
                    if (rawVal->Op == "+") {
                        return this->Builder->CreateAdd(
                            GeneratorValue(rawVal->Lhs, valueType),
                            GeneratorValue(rawVal->Rhs, valueType));
                    } else if (rawVal->Op == "-") {
                        return this->Builder->CreateSub(
                            GeneratorValue(rawVal->Lhs, valueType),
                            GeneratorValue(rawVal->Rhs, valueType));
                    } else if (rawVal->Op == "*") {
                        return this->Builder->CreateMul(
                            GeneratorValue(rawVal->Lhs, valueType),
                            GeneratorValue(rawVal->Rhs, valueType));
                    } else if (rawVal->Op == "/") {
                        return this->Builder->CreateSDiv(
                            GeneratorValue(rawVal->Lhs, valueType),
                            GeneratorValue(rawVal->Rhs, valueType));
                    }
                } else if (rawVal->Lhs) {
                    return GeneratorValue(rawVal->Lhs, valueType);
                } else {
                    return nullptr;
                }
            }
        }

    }  // namespace codegen
}  // namespace otter
