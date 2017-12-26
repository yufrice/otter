#include <codegen/generator.hpp>

namespace otter {
    namespace codegen {

        using namespace otter::ast;
        using namespace llvm;

        Generator::Generator()
            : Builder(new IRBuilder<>(context.get())),
              Module(new llvm::Module("OtterModule", context.get())){
                  logger._assert("Build Module");
              };

        Generator::~Generator() { delete this->Builder; }

        llvm::Instruction* Generator::addModuleInst(llvm::Instruction* inst,
                                                    bool flag) {
            // if (flag) {
            this->Builder->Insert(inst);
            // } else {
            //     auto retEnd = --(this->Entry->end());
            //     this->Entry->getInstList().insert(retEnd, inst);
            // }
            return inst;
        }

        std::unique_ptr<Module> Generator::generatorModule(
            const std::shared_ptr<moduleAST>& mod) {
            auto t1 = llvm::Type::getInt32Ty(this->context.get());

            FunctionType* funcType =
                FunctionType::get(llvm::Type::getVoidTy(context.get()), false);
            Function* mainFunc =
                Function::Create(funcType, Function::ExternalLinkage, "main",
                                 this->Module.get());

            this->Entry = BasicBlock::Create(context.get(), "entry", mainFunc);
            Builder->SetInsertPoint(this->Entry);

            for (auto stmt : mod->Stmt) {
                if (detail::sharedIsa<variableAST>(stmt)) {
                    if (auto rawVar =
                            detail::shared4Shared<variableAST>(stmt)) {
                        if (auto inst = generateGlovalVariable(rawVar)) {
                            if (isa<StoreInst>(inst)) {
                                if (auto storeInst =
                                        dyn_cast<StoreInst>(inst)) {
                                    this->Entry->getInstList().insert(
                                        this->Entry->end(), storeInst);
                                }
                            }
                        } else {
                            throw std::string("type error");
                        }
                    }
                } else if (detail::sharedIsa<funcCallAST>(stmt)) {
                    if (auto rawCall =
                            detail::shared4Shared<funcCallAST>(stmt)) {
                        if (auto inst = (generateCallFunc(rawCall))) {
                            this->Entry->getInstList().insert(
                                this->Entry->end(), inst);
                        } else {
                            throw std::string("type error");
                        }
                    }
                }
            }

            Builder->CreateRetVoid();
            return std::move(this->Module);
        }

        Value* Generator::generateGlovalVariable(
            const std::shared_ptr<variableAST>& var) {
            logger._assert("Add Global Variable : " + var->Name);
            if (detail::sharedIsa<functionAST>(var->Val)) {
                return this->GeneratorFunction(var);
            } else if (detail::sharedIsa<listAST>(var->Val)) {
                throw std::string("wip");
            } else if (detail::sharedIsa<ifStatementAST>(var->Val)) {
                auto gvar = new GlobalVariable(
                    *this->Module,
                    detail::type2type(var->Type, this->context.get()), false,
                    GlobalVariable::LinkageTypes::PrivateLinkage, nullptr,
                    var->Name);
                gvar->setInitializer(
                    detail::constantGet(var->Type, this->context.get()));
                this->Builder->CreateStore(this->generateifStmt(var->Val),
                                           gvar);
                return gvar;
            } else if (detail::sharedIsa<boolAST>(var->Val)) {
                if (auto rawBool = detail::sharedCast<boolAST>(var->Val)) {
                    auto constant = ConstantInt::get(
                        llvm::Type::getInt1Ty(this->context.get()),
                        static_cast<int>(rawBool->Bl));
                    auto gvar = new GlobalVariable(
                        *this->Module,
                        llvm::Type::getInt1Ty(this->context.get()), false,
                        GlobalVariable::LinkageTypes::PrivateLinkage, nullptr,
                        var->Name);
                    gvar->setInitializer(constant);
                    return gvar;
                }
            } else if (var->Type == TypeID::String) {
                if (detail::sharedIsa<stringAST>(var->Val)) {
                    return this->GeneratorGlobalString(var);
                } else if (detail::sharedIsa<identifierAST>(var->Val)) {
                    if (auto rawID =
                            detail::sharedCast<identifierAST>(var->Val)) {
                        auto id = this->Module->getGlobalVariable(rawID->Ident);
                        if (var->Type ==
                            detail::type2type(id->getValueType(),
                                              this->context.get())) {
                            throw detail::typeError(
                                "invConv", ast::getType(var->Type),
                                ast::getType(detail::type2type(
                                    id->getValueType(), this->context.get())),
                                rawID->Ident);
                        }
                        auto gvar = new GlobalVariable(
                            *this->Module, id->getValueType(), false,
                            GlobalVariable::LinkageTypes::PrivateLinkage,
                            nullptr, var->Name);
                        gvar->setInitializer(id);
                        return gvar;
                    }
                }
            } else {
                Constant* constant;
                double init = 0;
                if (detail::sharedIsa<numberAST>(var->Val)) {
                    if (auto rawNum = detail::sharedCast<numberAST>(var->Val)) {
                        if (var->Type != rawNum->Type) {
                            throw detail::typeError(
                                "invConv", ast::getType(var->Type),
                                ast::getType(rawNum->Type), var->Name);
                        }
                        init = rawNum->Val;
                    }
                }

                if (var->Type == TypeID::Int) {
                    constant = llvm::ConstantInt::getSigned(
                        Type::getInt32Ty(context.get()), init);
                } else if (var->Type == TypeID::Double) {
                    constant = llvm::ConstantFP::get(
                        Type::getDoubleTy(context.get()), init);
                }
                auto gvar = new GlobalVariable(
                    *this->Module, detail::type2type(var->Type, context.get()),
                    false, GlobalVariable::LinkageTypes::PrivateLinkage,
                    nullptr, var->Name);
                if (detail::sharedIsa<numberAST>(var->Val)) {
                    gvar->setInitializer(constant);
                    return constant;
                }
                gvar->setInitializer(constant);
                Value* value = this->GeneratorGlobalValue(var->Val);
                if (detail::type2type(value->getType(), this->context.get()) !=
                    var->Type) {
                    throw detail::typeError(
                        "invConv", ast::getType(detail::type2type(
                                       value->getType(), this->context.get())),
                        ast::getType(var->Type), var->Name);
                }
                return new StoreInst(value, gvar);
            }
        }

        Value* Generator::generateLise(const std::shared_ptr<baseAST>& ast) {}

        Value* Generator::generateifStmt(const std::shared_ptr<baseAST>& ast) {
            if (auto ifStmt = detail::sharedCast<ifStatementAST>(ast)) {
                auto currentBB    = this->Builder->GetInsertBlock();
                auto currentInstP = this->Builder->GetInsertPoint();

                auto cond    = this->Generator::generateCond(ifStmt->Cond);
                auto thenVal = GeneratorStatement(ifStmt->thenStmt);
                auto falseBB = BasicBlock::Create(
                    this->context.get(), "if.false", currentBB->getParent());
                auto newEntry = BasicBlock::Create(this->context.get(), "entry",
                                                   currentBB->getParent());
                this->Builder->CreateCondBr(cond, newEntry, falseBB);
                auto tmpEnt = this->Entry;

                this->Builder->SetInsertPoint(falseBB);
                this->Entry   = falseBB;
                auto falseVal = GeneratorStatement(ifStmt->falseStmt);
                if (thenVal->getType() != falseVal->getType()) {
                    throw std::string("dame");
                }
                this->Entry = newEntry;
                this->Builder->CreateBr(this->Entry);

                this->Builder->SetInsertPoint(this->Entry);
                auto phi = this->Builder->CreatePHI(thenVal->getType(), 2);
                phi->addIncoming(thenVal, tmpEnt);
                phi->addIncoming(falseVal, falseBB);

                return phi;
            }
        }

        Value* Generator::generateCond(const std::shared_ptr<baseAST>& cond) {
            if (auto rawBool = detail::sharedCast<boolAST>(cond)) {
                return ConstantInt::get(
                    llvm::Type::getInt1Ty(this->context.get()),
                    static_cast<int>(rawBool->Bl));
            } else if (detail::sharedIsa<identifierAST>(cond)) {
                auto condVal = GeneratorGlobalValue(cond);
                if (condVal->getType()->isIntegerTy(1)) {
                    return condVal;
                } else {
                    throw std::string("dame");
                }
            } else if (detail::sharedIsa<funcCallAST>(cond)) {
                return addModuleInst(generateCallFunc(cond),
                                     this->context.getCFunc());
            }
        }

        GlobalVariable* Generator::GeneratorGlobalString(
            std::shared_ptr<variableAST> var) {
            if (auto rawStr = detail::sharedCast<stringAST>(var->Val)) {
                ArrayType* Type =
                    ArrayType::get(IntegerType::get(context.get(), 8),
                                   (rawStr->Str).length() + 1);
                auto Name = var->Name;
                GlobalVariable* gvar =
                    new GlobalVariable(*this->Module, Type, true,
                                       GlobalValue::PrivateLinkage, 0, Name);

                Constant* strCons =
                    ConstantDataArray::getString(context.get(), rawStr->Str);

                gvar->setInitializer(strCons);
                return gvar;
            }
        }

        Instruction* Generator::generateCallFunc(
            const std::shared_ptr<baseAST>& expr) {
            std::vector<llvm::Value*> args;
            if (auto rawCall = detail::sharedCast<funcCallAST>(expr)) {
                if (rawCall->Name == "print") {
                    return generatePrint(expr);
                } else if (rawCall->Name == "=" | rawCall->Name == ">" |
                           rawCall->Name == "<" | rawCall->Name == ">=" |
                           rawCall->Name == "<=" | rawCall->Name == "<>") {
                    return generateLOp(expr);
                } else {
                    if (auto cf = this->Module->getFunction(rawCall->Name)) {
                        for (auto arg = cf->arg_begin(); arg != cf->arg_end();
                             ++arg) {
                            this->context.setType(arg->getType());
                        }
                        for (auto arg : rawCall->Args) {
                            args.emplace_back(GeneratorStatement(arg));
                        }
                        return CallInst::Create(cf, args);
                    } else {
                        throw std::string(rawCall->Name + " was not declar");
                    }
                }
            }
        }

        Instruction* Generator::generateLOp(
            const std::shared_ptr<baseAST>& expr) {
            if (auto rawCall = detail::sharedCast<funcCallAST>(expr)) {
                if (rawCall->Args.size() > 2) {
                    throw std::string("too many arguments to function op(any)");
                } else if (rawCall->Args.size() == 0) {
                    throw std::string("too few arguments to function op(any)");
                }
                auto args0 = rawCall->Args.at(0);
                auto args1 = rawCall->Args.at(1);
                auto lhs   = GeneratorStatement(args0);
                auto rhs   = GeneratorStatement(args1);

                Instruction::OtherOps type;

                if (lhs->getType() != rhs->getType()) {
                    throw std::string("dame");
                } else if (lhs->getType()->isIntegerTy()) {
                    type = Instruction::ICmp;
                } else if (lhs->getType()->isDoubleTy()) {
                    type = Instruction::FCmp;
                } else {
                    throw std::string("dame");
                }

                return CmpInst::Create(
                    type,
                    detail::op2lop(
                        rawCall->Name,
                        detail::type2type(lhs->getType(), this->context.get())),
                    lhs, rhs);
            }
        }

        CallInst* Generator::generatePrint(
            const std::shared_ptr<baseAST>& expr) {
            if (auto rawCall = detail::sharedCast<funcCallAST>(expr)) {
                if (rawCall->Args.size() != 1) {
                    throw std::string(
                        "too many arguments to function print(any)");
                } else if (rawCall->Args.size() == 0) {
                    throw std::string(
                        "too few arguments to function print(any)");
                }

                if (auto args = rawCall->Args.at(0)) {
                    auto val  = GeneratorStatement(args);
                    auto type = val->getType();
                    if (type->isPointerTy()) {
                        if (type->getPointerElementType()->isFunctionTy()) {
                            throw std::string("has no value print(any)");
                        }
                    }
                    std::string format;
                    detail::stdOutType(detail::pointerType(type), format);

                    llvm::Value* fInst;
                    if (!this->context.resolveFormat(format)) {
                        fInst = Builder->CreateGlobalStringPtr(
                            format, detail::formatNameMangling(format));
                    } else {
                        auto vTable = this->Module->getValueSymbolTable();
                        auto fName  = detail::formatNameMangling(format);
                        if (auto id = vTable.lookup(fName)) {
                            fInst = this->Builder->CreateInBoundsGEP(
                                id, std::vector<Value*>(
                                        2, ConstantInt::getSigned(
                                               llvm::Type::getInt32Ty(
                                                   this->context.get()),
                                               0)));
                        }
                    }
                    std::vector<Value*> argValue(1, fInst);
                    if (detail::sharedIsa<identifierAST>(args) &&
                        type->isPointerTy()) {
                        if (type->getPointerElementType()->isIntegerTy(32) ||
                            type->getPointerElementType()->isDoubleTy()) {
                            val = this->Builder->CreateLoad(val);
                        }
                    }

                    if (detail::pointerType(type)->isIntegerTy(1)) {
                        val           = this->Builder->CreateLoad(val);
                        auto constant = detail::i12Bool(val);

                        ArrayType* Type =
                            ArrayType::get(IntegerType::get(context.get(), 8),
                                           constant.length() + 2);
                        Constant* strCons = ConstantDataArray::getString(
                            context.get(), constant);
                        auto alloca = this->Builder->CreateAlloca(Type, 0, "");
                        this->Builder->CreateStore(
                            dyn_cast<llvm::Value>(strCons), alloca);
                        val = alloca;
                    }

                    argValue.emplace_back(val);
                    FunctionType* func_type = FunctionType::get(
                        Type::getInt32Ty(context.get()),
                        PointerType::get(llvm::Type::getInt8Ty(context.get()),
                                         0),
                        true);
                    Function* pFunc = dyn_cast<Function>(
                        this->Module->getOrInsertFunction("printf", func_type));
                    return CallInst::Create(pFunc, argValue);
                }
            }
        }

        Function* Generator::GeneratorFunction(
            std::shared_ptr<variableAST> var) {
            FunctionType* Type;
            std::vector<llvm::Type*> typeArgs;
            if (auto rawVal = detail::sharedCast<functionAST>(var->Val)) {
                for (auto argType : rawVal->Types) {
                    if (argType == TypeID::Int) {
                        typeArgs.emplace_back(Type::getInt32Ty(context.get()));
                    } else if (argType == TypeID::Double) {
                        typeArgs.emplace_back(Type::getDoubleTy(context.get()));
                    } else if (argType == TypeID::Bool) {
                        typeArgs.emplace_back(Type::getInt1Ty(context.get()));
                    } else if (argType == TypeID::String) {
                        typeArgs.emplace_back(PointerType::get(
                            Type::getInt8Ty(context.get()), 0));
                    }
                }
                ArrayRef<llvm::Type*> argsRef(typeArgs);
                if (var->Type == TypeID::Int) {
                    Type = FunctionType::get(
                        llvm::Type::getInt32Ty(context.get()), argsRef, false);
                } else if (var->Type == TypeID::Double) {
                    Type = FunctionType::get(
                        llvm::Type::getDoubleTy(context.get()), argsRef, false);
                } else if (var->Type == TypeID::String) {
                    Type = FunctionType::get(
                        PointerType::get(Type::getInt8Ty(context.get()), 0),
                        argsRef, false);
                } else if (var->Type == TypeID::Bool) {
                    Type = FunctionType::get(
                        llvm::Type::getInt1Ty(context.get()), argsRef, false);
                } else if (var->Type == TypeID::Unit) {
                    Type = FunctionType::get(
                        llvm::Type::getVoidTy(context.get()), argsRef, false);
                }

                auto func =
                    Function::Create(Type, llvm::Function::PrivateLinkage,
                                     var->Name, this->Module.get());
                auto argItr = func->arg_begin();
                for (auto arg : rawVal->Args) {
                    (argItr++)->setName(arg);
                }
                BasicBlock* bblock =
                    BasicBlock::Create(context.get(), "entry", func);
                Builder->SetInsertPoint(bblock);
                Value* ret;

                this->context.setCFunc(true);
                auto mEntry = this->Entry;
                this->Entry = bblock;

                for (auto& st : rawVal->Statements) {
                    ret = GeneratorStatement(st);
                }
                this->context.setCFunc(false);
                if (var->Type == TypeID::Unit) {
                    Builder->CreateRetVoid();
                    this->Builder->SetInsertPoint(mEntry);
                    this->Entry = mEntry;
                    return func;
                } else {
                    if (ret->getType()->isPointerTy()) {
                        auto retLoad = this->Builder->CreateLoad(ret);
                        this->Builder->CreateRet(retLoad);
                    } else {
                        this->Builder->CreateRet(ret);
                    }
                }
                if (detail::pointerType(ret->getType()) !=
                    detail::type2type(var->Type, this->context.get())) {
                    throw detail::typeError(
                        "invConv", getType(detail::type2type(
                                       ret->getType(), this->context.get())),
                        getType(var->Type), "ret");
                }
                this->Builder->SetInsertPoint(mEntry);
                this->Entry = mEntry;
                return func;
            }
        }

        Value* Generator::GeneratorStatement(std::shared_ptr<baseAST> stmt) {
            auto vTable = this->Entry->getValueSymbolTable();
            if (detail::sharedIsa<funcCallAST>(stmt)) {
                return addModuleInst(generateCallFunc(stmt),
                                     this->context.getCFunc());
            } else if (detail::sharedIsa<identifierAST>(stmt)) {
                auto gvTable = this->Module->getValueSymbolTable();
                llvm::Value* val;
                if (auto rawID = detail::sharedCast<identifierAST>(stmt)) {
                    if (auto id = vTable->lookup(rawID->Ident)) {
                        return id;
                    } else if (auto id = gvTable.lookup(rawID->Ident)) {
                        return this->Builder->CreateLoad(id);
                        val = id;
                    } else {
                        throw std::string(rawID->Ident + " was not declar");
                    }
                    return val;
                }
            } else if (detail::sharedIsa<variableAST>(stmt)) {
                return generateVariable(
                    detail::shared4Shared<variableAST>(stmt), vTable);
            } else if (detail::sharedIsa<binaryExprAST>(stmt)) {
                return this->GeneratorGlobalValue(stmt, vTable);
            } else if (detail::sharedIsa<numberAST>(stmt)) {
                if (auto rawNum = detail::sharedCast<numberAST>(stmt)) {
                    if (ast::TypeID::Double == rawNum->Type) {
                        return llvm::ConstantFP::get(
                            llvm::Type::getDoubleTy(this->context.get()),
                            rawNum->Val);
                    } else {
                        return llvm::ConstantInt::get(
                            llvm::Type::getInt32Ty(this->context.get()),
                            rawNum->Val);
                    }
                }
            } else if (detail::sharedIsa<stringAST>(stmt)) {
                if (auto rawStr = detail::sharedCast<stringAST>(stmt)) {
                    return Builder->CreateGlobalStringPtr(rawStr->Str);
                }
            } else if (detail::sharedIsa<ifStatementAST>(stmt)) {
                return generateifStmt(stmt);
            }
        }

        Value* Generator::generateVariable(
            const std::shared_ptr<variableAST>& var,
            auto vTable) {
            if (detail::sharedIsa<functionAST>(var->Val)) {
                return this->GeneratorFunction(var);
            } else if (detail::sharedIsa<stringAST>(var->Val)) {
                if (TypeID::String == var->Type) {
                    return this->generateString(var);
                }
            } else if (detail::sharedIsa<ifStatementAST>(var->Val)) {
                auto constant = generateifStmt(var->Val);
                auto alloca   = this->Builder->CreateAlloca(constant->getType(),
                                                          0, var->Name);
                this->Builder->CreateStore(constant, alloca);
                return vTable->lookup(var->Name);
            }

            Value* constant;
            if (detail::sharedIsa<identifierAST>(var->Val)) {
                if (auto rawID = detail::sharedCast<identifierAST>(var->Val)) {
                    if (auto id = vTable->lookup(rawID->Ident)) {
                        constant = id;
                    } else if (auto id =
                                   this->Module->getValueSymbolTable().lookup(
                                       rawID->Ident)) {
                        constant = id;
                    } else {
                        throw std::string(rawID->Ident + " was not declar");
                    }
                    if (detail::pointerType(constant->getType()) !=
                        detail::type2type(var->Type, this->context.get())) {
                        throw detail::typeError(
                            "invConv",
                            ast::getType(detail::type2type(
                                detail::pointerType(constant->getType()),
                                this->context.get())),
                            ast::getType(var->Type), var->Name);
                    }
                }
            } else if (detail::sharedIsa<binaryExprAST>(var->Val)) {
                constant = GeneratorGlobalValue(var->Val, vTable);
            } else if (detail::sharedIsa<numberAST>(var->Val)) {
                constant =
                    detail::constantGet(std::move(var->Val), context.get());
            }
            if (constant == nullptr) {
                throw "";
            }
            auto alloca = this->Builder->CreateAlloca(
                detail::type2type(var->Type, context.get()), 0, var->Name);
            this->Builder->CreateStore(constant, alloca);
            return vTable->lookup(var->Name);
        }

        Value* Generator::generateString(std::shared_ptr<variableAST> var) {
            if (auto rawStr = detail::sharedCast<stringAST>(var->Val)) {
                ArrayType* Type =
                    ArrayType::get(IntegerType::get(context.get(), 8),
                                   (rawStr->Str).length() + 2);
                auto Name = var->Name;
                Constant* strCons =
                    ConstantDataArray::getString(context.get(), rawStr->Str);
                auto alloca = this->Builder->CreateAlloca(Type, 0, Name);
                return this->Builder->CreateStore(
                    dyn_cast<llvm::Value>(strCons), alloca);
                return alloca;
            }
        }

        Value* Generator::GeneratorGlobalValue(
            const std::shared_ptr<baseAST>& var,
            const llvm::ValueSymbolTable* vTable) {
            if (auto rawVal = detail::sharedCast<identifierAST>(var)) {
                if (vTable != nullptr) {
                    if (auto id = vTable->lookup(rawVal->Ident)) {
                        return id;
                    }
                } else {
                    if (auto id = this->Entry->getValueSymbolTable()->lookup(
                            rawVal->Ident)) {
                        return id;
                    }
                }
                if (auto id = this->Module->getValueSymbolTable().lookup(
                        rawVal->Ident)) {
                    if (id->getType()
                            ->getPointerElementType()
                            ->isFunctionTy()) {
                        std::vector<llvm::Value*> args;
                        return addModuleInst(
                            CallInst::Create(
                                this->Module->getFunction(rawVal->Ident), args),
                            this->context.getCFunc());
                    }
                    return addModuleInst(new LoadInst(id, ""),
                                         this->context.getCFunc());
                }
                throw std::string(rawVal->Ident + " was not declar");
            } else if (detail::sharedIsa<numberAST>(var)) {
                return detail::constantGet(std::move(var), this->context.get());
            } else if (detail::sharedIsa<funcCallAST>(var)) {
                return addModuleInst(generateCallFunc(var),
                                     this->context.getCFunc());
            } else if (auto rawVal = detail::sharedCast<binaryExprAST>(var)) {
                auto lhs = GeneratorGlobalValue(rawVal->Lhs, std::move(vTable));
                auto rhs = GeneratorGlobalValue(rawVal->Rhs, std::move(vTable));
                return addModuleInst(
                    BinaryOperator::Create(
                        detail::op2op(rawVal->Op,
                                      detail::type2type(lhs->getType(),
                                                        this->context.get())),
                        lhs, rhs),
                    this->context.getCFunc());
            } else if (rawVal->Lhs) {
                return GeneratorGlobalValue(rawVal->Lhs, std::move(vTable));
            } else {
                return nullptr;
            }
        }

    }  // namespace codegen
}  // namespace otter
