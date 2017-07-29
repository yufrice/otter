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
                llvm::Type::getVoidTy(this->TheContext), false);
            Function* mainFunc = Function::Create(
                funcType, Function::ExternalLinkage, "main", this->Module);

            BasicBlock* entry =
                BasicBlock::Create(this->TheContext, "entry", mainFunc);
            Builder->SetInsertPoint(entry);
            Builder->CreateRetVoid();

            for (auto var : mod->Vars) {
                if (generateGlovalVariable(var)) {
                    // throw std::string("type error");
                }
            }

            // test code
            // auto args = PointerType::get(Type::getInt8Ty(this->TheContext),
            // 0);
            // llvm::Value* msg        = Builder->CreateGlobalStringPtr("%d\n");
            // FunctionType* func_type = FunctionType::get(
            //     Type::getInt32Ty(this->TheContext), args, true);
            // Function* F = dyn_cast<Function>(
            //     this->Module->getOrInsertFunction("printf", func_type));
            // std::vector<llvm::Value*> ar;
            // ar.push_back(msg);
            // auto a = this->Builder->CreateLoad(
            //     (this->Module->getGlobalVariable("b")));
            // ar.push_back(a);
            // this->Builder->CreateCall(F, ar);

            Module->dump();
            return this->Module;
        }

        Value* Generator::generateGlovalVariable(
            std::shared_ptr<variableAST> var) {
            if (detail::sharedIsa<functionAST>(var->Val)) {
                return this->GeneratorFunction(var, var->Type);
            } else if (detail::sharedIsa<stringAST>(var->Val)) {
                if (TypeID::String == var->Type) {
                    return this->GeneratorGlobalString(var);
                }
            } else if (detail::sharedIsa<identifierAST>(var->Val)) {
                std::cout << "id" << std::endl;
            } else {
                Type* valueType;
                Constant* constant;
                Value* value;
                if (var->Type == TypeID::Int) {
                    valueType = Type::getInt32Ty(this->TheContext);
                    constant  = llvm::ConstantInt::getSigned(valueType, 0);
                } else {
                    valueType = Type::getDoubleTy(this->TheContext);
                    constant  = llvm::ConstantFP::get(valueType, 0.0);
                }
                auto gvar = new GlobalVariable(
                    *this->Module, valueType, false,
                    GlobalVariable::LinkageTypes::CommonLinkage, nullptr,
                    var->Name);
                gvar->setInitializer(constant);
                value = this->GeneratorValue(var->Val, var->Type);
                return this->Builder->CreateStore(value, gvar);
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

                Constant* strCons = ConstantDataArray::getString(
                    this->TheContext, rawStr->Str + "\n");

                gvar->setInitializer(strCons);
                return gvar;
            }
        }

        Function* Generator::GeneratorFunction(std::shared_ptr<variableAST> var,
                                               TypeID funcType) {
            FunctionType* Type;
            std::vector<llvm::Type*> typeArgs;
            if (auto rawVal = detail::sharedCast<functionAST>(var->Val)) {
                auto argType = rawVal->Types.begin();
                for (auto args = rawVal->Args.begin();
                     args != rawVal->Args.end(); ++argType, ++args) {
                    if (*argType == TypeID::Int) {
                        typeArgs.emplace_back(
                            Type::getInt32Ty(this->TheContext));
                    } else if (*argType == TypeID::Double) {
                        typeArgs.emplace_back(
                            Type::getDoubleTy(this->TheContext));
                    } else if (*argType == TypeID::String) {
                        typeArgs.emplace_back(PointerType::get(
                            Type::getInt8Ty(this->TheContext), 0));
                    }
                }
                ArrayRef<llvm::Type*> argsRef(typeArgs);
                if (funcType == TypeID::Int) {
                    Type = FunctionType::get(
                        llvm::Type::getInt32Ty(this->TheContext), argsRef,
                        false);
                } else if (funcType == TypeID::Double) {
                    Type = FunctionType::get(
                        llvm::Type::getDoubleTy(this->TheContext), argsRef,
                        false);
                } else if (funcType == TypeID::String) {
                    Type = FunctionType::get(
                        PointerType::get(Type::getInt8Ty(this->TheContext), 0),
                        argsRef, false);
                } else if (funcType == TypeID::Unit) {
                    Type = FunctionType::get(
                        llvm::Type::getVoidTy(this->TheContext), argsRef,
                        false);
                }

                auto func =
                    Function::Create(Type, llvm::Function::ExternalLinkage,
                                     var->Name, this->Module);
                BasicBlock* bblock =
                    BasicBlock::Create(this->TheContext, "entry", func);
                Builder->SetInsertPoint(bblock);
                Value* ret;
                for (auto& st : rawVal->Statements) {
                    ret = GeneratorStatement(st);
                }

                if (funcType == TypeID::Unit) {
                    Builder->CreateRetVoid();
                } else if (funcType == TypeID::String) {
                    Builder->CreateRet(ret);
                } else {
                    Builder->CreateRet(ret);
                }
                return func;
            }
        }

        Value* Generator::GeneratorStatement(std::shared_ptr<baseAST> stmt) {
            if (detail::sharedIsa<funcCallAST>(stmt)) {
                std::cout << "call" << std::endl;
            } else if (detail::sharedIsa<variableAST>(stmt)) {
                return generateVariable(
                    detail::shared4Shared<variableAST>(stmt));
            }
        }

        Value* Generator::generateVariable(std::shared_ptr<variableAST> var) {
            std::cout << var->Name << std::endl;
            if (detail::sharedIsa<functionAST>(var->Val)) {
                return this->GeneratorFunction(var, var->Type);
            } else if (detail::sharedIsa<stringAST>(var->Val)) {
                if (TypeID::String == var->Type) {
                    return this->generateString(var);
                }
                // } else if (detail::sharedIsa<identifierAST>(var->Val)) {
                //     std::cout << "id" << std::endl;
                // } else {
                //     Type* valueType;
                //     Constant* constant;
                //     Value* value;
                //     if (var->Type == TypeID::Int) {
                //         valueType = Type::getInt32Ty(this->TheContext);
                //         constant  =
                //         llvm::ConstantInt::getSigned(valueType,
                //         0);
                //     } else {
                //         valueType = Type::getDoubleTy(this->TheContext);
                //         constant  = llvm::ConstantFP::get(valueType,
                //         0.0);
                //     }
                //     auto gvar = new GlobalVariable(
                //         *this->Module, valueType, false,
                //         GlobalVariable::LinkageTypes::CommonLinkage,
                //         nullptr,
                //         var->Name);
                //     gvar->setInitializer(constant);
                //     value = this->GeneratorValue(var->Val, var->Type);
                //     this->Builder->CreateStore(value, gvar);
                //     return true;
            }
        }

        Value* Generator::generateString(std::shared_ptr<variableAST> var) {
            if (auto rawStr = detail::sharedCast<stringAST>(var->Val)) {
                ArrayType* Type =
                    ArrayType::get(IntegerType::get(this->TheContext, 8),
                                   (rawStr->Str).length() + 2);
                auto Name = var->Name;
                auto var  = Builder->CreateAlloca(Type, 0, Name);

                Constant* strCons = ConstantDataArray::getString(
                    this->TheContext, rawStr->Str + "\n");

                Builder->CreateStore(strCons, var);
                return var;
            }
        }

        Value* Generator::GeneratorValue(std::shared_ptr<baseAST> var,
                                         TypeID type) {
            llvm::Type* valueType;
            if (type == TypeID::Int) {
                valueType = llvm::Type::getInt32Ty(this->TheContext);
            } else {
                valueType = llvm::Type::getDoubleTy(this->TheContext);
            }

            if (auto rawVal = detail::sharedCast<identifierAST>(var)) {
                return this->Builder->CreateLoad(
                    this->Module->getGlobalVariable(rawVal->Ident), "");
            } else if (detail::sharedIsa<numberAST>(var)) {
                return detail::constantGet(std::move(var), type,
                                           this->TheContext);
            } else if (auto rawVal = detail::sharedCast<binaryExprAST>(var)) {
                if (rawVal->Rhs) {
                    if (rawVal->Op == "+") {
                        return this->Builder->CreateFAdd(
                            GeneratorValue(rawVal->Lhs, type),
                            GeneratorValue(rawVal->Rhs, type));
                    } else if (rawVal->Op == "-") {
                        return this->Builder->CreateFSub(
                            GeneratorValue(rawVal->Lhs, type),
                            GeneratorValue(rawVal->Rhs, type));
                    } else if (rawVal->Op == "*") {
                        return this->Builder->CreateFMul(
                            GeneratorValue(rawVal->Lhs, type),
                            GeneratorValue(rawVal->Rhs, type));
                    } else if (rawVal->Op == "/") {
                        return this->Builder->CreateFDiv(
                            GeneratorValue(rawVal->Lhs, type),
                            GeneratorValue(rawVal->Rhs, type));
                    }
                } else if (rawVal->Lhs) {
                    return GeneratorValue(rawVal->Lhs, type);
                } else {
                    return nullptr;
                }
            }
        }

    }  // namespace codegen
}  // namespace otter
