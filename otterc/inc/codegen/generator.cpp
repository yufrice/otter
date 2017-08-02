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

            for (auto var : mod->Vars) {
                if (!generateGlovalVariable(var)) {
                    throw std::string("type error");
                }
            }

            for (auto func : mod->Funcs) {
                if (auto inst = (generateCallFunc(func))) {
                    entry->getInstList().insert(entry->end(), inst);
                } else {
                    throw std::string("type error");
                }
            }

            auto retVoid = ReturnInst::Create(this->TheContext);
            entry->getInstList().insert(entry->end(), retVoid);

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
                return this->GeneratorFunction(var);
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

        CallInst* Generator::generateCallFunc(std::shared_ptr<baseAST> expr) {
            std::vector<llvm::Value*> args;
            if (auto rawCall = detail::sharedCast<funcCallAST>(expr)) {
                if (rawCall->Name == "print") {
                } else {
                    return CallInst::Create(
                        this->Module->getFunction(rawCall->Name), args,
                        rawCall->Name);
                }
            }
        }

        Function* Generator::GeneratorFunction(
            std::shared_ptr<variableAST> var) {
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
                if (var->Type == TypeID::Int) {
                    Type = FunctionType::get(
                        llvm::Type::getInt32Ty(this->TheContext), argsRef,
                        false);
                } else if (var->Type == TypeID::Double) {
                    Type = FunctionType::get(
                        llvm::Type::getDoubleTy(this->TheContext), argsRef,
                        false);
                } else if (var->Type == TypeID::String) {
                    Type = FunctionType::get(
                        PointerType::get(Type::getInt8Ty(this->TheContext), 0),
                        argsRef, false);
                } else if (var->Type == TypeID::Unit) {
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
                    ret = GeneratorStatement(st, func);
                }
                if (var->Type == TypeID::Unit) {
                    Builder->CreateRetVoid();
                } else {
                    Builder->CreateRet(ret);
                }
                return func;
            }
        }

        Value* Generator::GeneratorStatement(std::shared_ptr<baseAST> stmt,
                                             Function* func) {
            if (detail::sharedIsa<funcCallAST>(stmt)) {
                return generateCallFunc(stmt);
            } else if (detail::sharedIsa<variableAST>(stmt)) {
                return generateVariable(
                    detail::shared4Shared<variableAST>(stmt));
            } else if (detail::sharedIsa<binaryExprAST>(stmt)) {
                TypeID type;
                if (llvm::Type::getDoubleTy(this->TheContext) ==
                    func->getReturnType()) {
                    type = TypeID::Double;
                } else if (llvm::Type::getInt32Ty(this->TheContext) ==
                           func->getReturnType()) {
                    type = TypeID::Int;
                }
                auto value = this->GeneratorValue(stmt, type);
                return Builder->CreateAlloca(func->getReturnType(), value,
                                             "ret");
            } else if (detail::sharedIsa<stringAST>(stmt)) {
                if (auto rawStr = detail::sharedCast<stringAST>(stmt)) {
                    return ConstantDataArray::getString(this->TheContext,
                                                        rawStr->Str + "\n");
                }
            }
        }

        Value* Generator::generateVariable(std::shared_ptr<variableAST> var) {
            if (detail::sharedIsa<functionAST>(var->Val)) {
                return this->GeneratorFunction(var);
            } else if (detail::sharedIsa<stringAST>(var->Val)) {
                if (TypeID::String == var->Type) {
                    return this->generateString(var);
                }
            } else if (detail::sharedIsa<identifierAST>(var->Val)) {
                std::cout << "id" << std::endl;
            } else if (detail::sharedIsa<binaryExprAST>(var->Val)) {
                std::cout << "expr" << std::endl;
                //     Type* valueType;
                //     Constant* constant;
                //     Value* value;
                //     if (var->Type == TypeID::Int) {
                //         valueType =
                //         Type::getInt32Ty(this->TheContext);
                //         constant  =
                //         llvm::ConstantInt::getSigned(valueType,
                //         0);
                //     } else {
                //         valueType =
                //         Type::getDoubleTy(this->TheContext);
                //         constant  = llvm::ConstantFP::get(valueType,
                //         0.0);
                //     }
                //     auto gvar = new GlobalVariable(
                //         *this->Module, valueType, false,
                //         GlobalVariable::LinkageTypes::CommonLinkage,
                //         nullptr,
                //         var->Name);
                //     gvar->setInitializer(constant);
                //     value = this->GeneratorValue(var->Val,
                //     var->Type);
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
            std::cout << "\n kaisou ";
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
                    std::cout << rawVal->Op << " ";
                    if (rawVal->Op == "+") {
                        if (type == TypeID::Int) {
                            return this->Builder->CreateAdd(
                                GeneratorValue(rawVal->Lhs, type),
                                GeneratorValue(rawVal->Rhs, type));
                        } else {
                            return this->Builder->CreateFAdd(
                                GeneratorValue(rawVal->Lhs, type),
                                GeneratorValue(rawVal->Rhs, type));
                        }
                    } else if (rawVal->Op == "-") {
                        if (type == TypeID::Int) {
                            return this->Builder->CreateSub(
                                GeneratorValue(rawVal->Lhs, type),
                                GeneratorValue(rawVal->Rhs, type));
                        } else {
                            return this->Builder->CreateFSub(
                                GeneratorValue(rawVal->Lhs, type),
                                GeneratorValue(rawVal->Rhs, type));
                        }
                    } else if (rawVal->Op == "*") {
                        if (type == TypeID::Int) {
                            return this->Builder->CreateMul(
                                GeneratorValue(rawVal->Lhs, type),
                                GeneratorValue(rawVal->Rhs, type));
                        } else {
                            return this->Builder->CreateFMul(
                                GeneratorValue(rawVal->Lhs, type),
                                GeneratorValue(rawVal->Rhs, type));
                        }
                    } else if (rawVal->Op == "/") {
                        if (type == TypeID::Int) {
                            return this->Builder->CreateSDiv(
                                GeneratorValue(rawVal->Lhs, type),
                                GeneratorValue(rawVal->Rhs, type));
                        } else {
                            return this->Builder->CreateFDiv(
                                GeneratorValue(rawVal->Lhs, type),
                                GeneratorValue(rawVal->Rhs, type));
                        }
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
