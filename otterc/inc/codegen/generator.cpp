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


       llvm::Instruction* Generator::addModuleInst(llvm::Instruction* inst){
            auto retEnd = --(this->Entry->end());
            this->Entry->getInstList().insert(retEnd, inst);
            return inst;
       }

        Module* Generator::generatorModule(std::shared_ptr<moduleAST> mod) {
            FunctionType* funcType = FunctionType::get(
                llvm::Type::getVoidTy(this->TheContext), false);
            Function* mainFunc = Function::Create(
                funcType, Function::ExternalLinkage, "main", this->Module);

            this->Entry =
                BasicBlock::Create(this->TheContext, "entry", mainFunc);
            Builder->SetInsertPoint(this->Entry);
            Builder->CreateRetVoid();
            auto retEnd = --(this->Entry->end());

            for (auto stmt: mod->Stmt) {
                if(detail::sharedIsa<variableAST>(stmt)){
                    if(auto rawVar = detail::shared4Shared<variableAST>(stmt)){
                        if (auto inst = generateGlovalVariable(rawVar)) {
                            if(isa<StoreInst>(inst)){
                                if(auto storeInst = dyn_cast<StoreInst>(inst)){
                                    this->Entry->getInstList().insert(retEnd, storeInst);
                                }
                            }
                        }else {
                            throw std::string("type error");
                        }
                    }
                }else if(detail::sharedIsa<funcCallAST>(stmt)){
                    if(auto rawCall = detail::shared4Shared<funcCallAST>(stmt)){
                        if (auto inst = (generateCallFunc(rawCall))) {
                            this->Entry->getInstList().insert(retEnd, inst);
                        } else {
                            throw std::string("type error");
                        }
                    }
                }
            }

            //auto retVoid = ReturnInst::Create(this->TheContext);
            //entry->getInstList().insert(entry->end(), retVoid);

            Module->dump();
            return this->Module;
        }

        Value* Generator::generateGlovalVariable(
            std::shared_ptr<variableAST> var) {
            if (detail::sharedIsa<functionAST>(var->Val)) {
                return this->GeneratorFunction(var);
            }else if (var->Type == TypeID::String) {
                if (detail::sharedIsa<stringAST>(var->Val)) {
                    return this->GeneratorGlobalString(var);
                }else if (detail::sharedIsa<identifierAST>(var->Val)) {
                    if(auto rawID = detail::sharedCast<identifierAST>(var->Val)){
                        auto id = this->Module->getGlobalVariable(rawID->Ident);
                        auto gvar = new GlobalVariable(
                           *this->Module, id->getValueType(), false,
                           GlobalVariable::LinkageTypes::PrivateLinkage, nullptr,
                           var->Name);
                        gvar->setInitializer(id);
                    }
                }
            } else {
                Constant* constant;
                double init = 0;
                if (detail::sharedIsa<numberAST>(var->Val)) {
                    if(auto rawNum = detail::sharedCast<numberAST>(var->Val)){
                        init = rawNum->Val;
                    }
                }

                if (var->Type == TypeID::Int) {
                    constant  = llvm::ConstantInt::getSigned(Type::getInt32Ty(this->TheContext),init);
                } else if(var->Type == TypeID::Double){
                    constant  = llvm::ConstantFP::get(Type::getDoubleTy(this->TheContext), init);
                }
                auto gvar = new GlobalVariable(
                    *this->Module, detail::type2type(var->Type,this->TheContext), false,
                    GlobalVariable::LinkageTypes::PrivateLinkage, nullptr,
                    var->Name);
                if (detail::sharedIsa<numberAST>(var->Val)) {
                        gvar->setInitializer(constant);
                        return constant;
                }
                gvar->setInitializer(constant);
                Value* value = this->GeneratorGlobalValue(var->Val, var->Type);
                return new StoreInst(value, gvar);
            }
        }

        GlobalVariable* Generator::GeneratorGlobalString(
            std::shared_ptr<variableAST> var) {
            if (auto rawStr = detail::sharedCast<stringAST>(var->Val)) {
                ArrayType* Type =
                    ArrayType::get(IntegerType::get(this->TheContext, 8),
                                   (rawStr->Str).length() + 1);
                auto Name = var->Name;
                GlobalVariable* gvar =
                    new GlobalVariable(*this->Module, Type, true,
                                       GlobalValue::PrivateLinkage, 0, Name);

                Constant* strCons = ConstantDataArray::getString(
                    this->TheContext, rawStr->Str);

                gvar->setInitializer(strCons);
                return gvar;
            }
        }

        CallInst* Generator::generateCallFunc(const std::shared_ptr<baseAST> &expr,const Function* func) {
            std::vector<llvm::Value*> args;
            if (auto rawCall = detail::sharedCast<funcCallAST>(expr)) {
                if (rawCall->Name == "print") {
                    return generatePrint(expr,func);
                } else {
                    return CallInst::Create(
                        this->Module->getFunction(rawCall->Name), args,
                        rawCall->Name);
                }
            }
        }

        CallInst* Generator::generatePrint(const std::shared_ptr<baseAST> &expr,const Function* func) {
            if (auto rawCall = detail::sharedCast<funcCallAST>(expr)) {
                    if(rawCall->Args.size() != 1){
                        throw std::string("too many arguments to function print(any)");
                    }else if(rawCall->Args.size() == 0){
                        throw std::string("too few arguments to function print(any)");
                    }

                    Value* val = nullptr;
                    llvm::Type* type;
                    std::string format;
                    FunctionType* func_type = FunctionType::get(Type::getInt32Ty(this->TheContext), PointerType::get(llvm::Type::getInt8Ty(this->TheContext),0), true);
                    Function* pFunc = dyn_cast<Function>(this->Module->getOrInsertFunction("printf", func_type));
                    if(auto args = rawCall->Args.at(0)){
                        if(detail::sharedIsa<identifierAST>(args)){
                            if(auto rawID = detail::sharedCast<identifierAST>(args)){
                                auto gvTable = this->Module->getValueSymbolTable();
                                if(auto id = gvTable.lookup(rawID->Ident)){
                                    val = id;
                                    type = val->getType();
                                }
                                if(func != nullptr){
                                    auto vTable = func->getValueSymbolTable();
                                    if(auto id = vTable->lookup(rawID->Ident)){
                                        val = id;
                                        type = val->getType();
                                    }
                                }
                                if(val == nullptr){
                                    throw std::string(rawID->Ident + " was not declar");
                                }
                                detail::stdOutType(type,format);
                            }
                        }else if(detail::sharedIsa<stringAST>(args)){
                            if(auto rawStr = detail::sharedCast<stringAST>(args)){
                                ArrayType* Type =
                                    ArrayType::get(IntegerType::get(this->TheContext, 8),
                                        (rawStr->Str).length() + 2);
                               Constant* strCons = ConstantDataArray::getString(
                                    this->TheContext, rawStr->Str);
                                    AllocaInst* allocaInst;
                                    if(func != nullptr){
                                        allocaInst = dyn_cast<AllocaInst>(addModuleInst (new AllocaInst(Type, strCons)));
                                    }else{
                                        allocaInst = dyn_cast<AllocaInst>(addModuleInst (new AllocaInst(Type, strCons)));
                                    }
                                format = "%s\n";
                                std::vector<Value*> argValue(1,Builder->CreateGlobalStringPtr(format));
                                argValue.push_back(allocaInst);
                                return CallInst::Create(pFunc, argValue);
                            }
                        }
                        std::vector<Value*> argValue(1,Builder->CreateGlobalStringPtr(format));
                        if(type->getPointerElementType()->getTypeID() == 14){
                            argValue.push_back(val);
                        }else{
                            auto loadInst = dyn_cast<LoadInst>(addModuleInst(new LoadInst(val)));
                            argValue.push_back(loadInst);
                        }
                        return CallInst::Create(pFunc, argValue);
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
                    Function::Create(Type, llvm::Function::PrivateLinkage,
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
                } else if(var->Type == TypeID::Int){
                    Builder->CreateRet(ret);
                } else {
                    Builder->CreateRet(ret);
                }
                return func;
            }
        }

        Value* Generator::GeneratorStatement(std::shared_ptr<baseAST> stmt,
                                             Function* func) {
            if (detail::sharedIsa<funcCallAST>(stmt)) {
                return generateCallFunc(stmt,func);
            } else if (detail::sharedIsa<identifierAST>(stmt)) {
                auto vTable = func->getValueSymbolTable();
                auto gvTable = this->Module->getValueSymbolTable();
                if(auto rawID = detail::sharedCast<identifierAST>(stmt)){
                    if(auto id = vTable->lookup(rawID->Ident)){
                        return id;
                    }else if(auto id = gvTable.lookup(rawID->Ident)){
                        return id;
                    }else{
                        throw std::string(rawID->Ident + " was not declar");
                    }
                }
            } else if (detail::sharedIsa<variableAST>(stmt)) {
                return generateVariable(
                    detail::shared4Shared<variableAST>(stmt),func->getValueSymbolTable());
            } else if (detail::sharedIsa<binaryExprAST>(stmt)) {
                TypeID type;
                if (llvm::Type::getDoubleTy(this->TheContext) ==
                    func->getReturnType()) {
                    type = TypeID::Double;
                } else if (llvm::Type::getInt32Ty(this->TheContext) ==
                           func->getReturnType()) {
                    type = TypeID::Int;
                }
                auto value = this->GeneratorGlobalValue(stmt, type,func->getValueSymbolTable());
                return value;
            } else if (detail::sharedIsa<numberAST>(stmt)) {
                if(auto rawNum = detail::sharedCast<numberAST>(stmt)){
                if (llvm::Type::getDoubleTy(this->TheContext) ==
                    func->getReturnType()) {
                        return llvm::ConstantFP::get(func->getReturnType() , rawNum->Val);
                    }else{
                        return llvm::ConstantInt::get(func->getReturnType() , rawNum->Val);
                    }
                }
            } else if (detail::sharedIsa<stringAST>(stmt)) {
                if (auto rawStr = detail::sharedCast<stringAST>(stmt)) {
                    return ConstantDataArray::getString(this->TheContext,
                                                        rawStr->Str);
                }
            }
        }

        Value* Generator::generateVariable(std::shared_ptr<variableAST> var,auto vTable) {
            if (detail::sharedIsa<functionAST>(var->Val)) {
                return this->GeneratorFunction(var);
            } else if (detail::sharedIsa<stringAST>(var->Val)) {
                if (TypeID::String == var->Type) {
                    return this->generateString(var);
                }
            }

            Value* constant;
            if (detail::sharedIsa<identifierAST>(var->Val)) {
                if(auto rawID = detail::sharedCast<identifierAST>(var->Val)){
                    if(auto id = vTable->lookup(rawID->Ident)){
                        constant = id;
                    }else if(auto id = this->Module->getValueSymbolTable().lookup(rawID->Ident)){
                        constant = id;
                    }else{
                        throw std::string(rawID->Ident + " was not declar");
                    }
                }
            } else if (detail::sharedIsa<binaryExprAST>(var->Val)) {
                constant = GeneratorGlobalValue(var->Val,var->Type,vTable);
            } else if (detail::sharedIsa<numberAST>(var->Val)) {
                constant = detail::constantGet(std::move(var->Val), var->Type,
                                           this->TheContext);
            }
            return this->Builder->CreateAlloca(detail::type2type(var->Type,this->TheContext),constant, var->Name);
        }

        Value* Generator::generateString(std::shared_ptr<variableAST> var) {
            if (auto rawStr = detail::sharedCast<stringAST>(var->Val)) {
                ArrayType* Type =
                    ArrayType::get(IntegerType::get(this->TheContext, 8),
                                   (rawStr->Str).length() + 2);
                auto Name = var->Name;
                Constant* strCons = ConstantDataArray::getString(
                    this->TheContext, rawStr->Str);
                return this->Builder->CreateAlloca(Type, strCons, Name);
            }
        }

        Value* Generator::GeneratorGlobalValue(std::shared_ptr<baseAST> var,
                                         TypeID type, llvm::ValueSymbolTable* vTable) {
            if (auto rawVal = detail::sharedCast<identifierAST>(var)) {
                if(auto id = vTable->lookup(rawVal->Ident)){
                    return this->Builder->CreateLoad(id, "");
                }else if(auto id = this->Module->getValueSymbolTable().lookup(rawVal->Ident)){
                    return this->Builder->CreateLoad(id, "");
                }
                throw std::string(rawVal->Ident + "was not declar");
            } else if (detail::sharedIsa<numberAST>(var)) {
                return detail::constantGet(std::move(var), type,
                                           this->TheContext);
            } else if (auto rawVal = detail::sharedCast<binaryExprAST>(var)) {
                if (rawVal->Rhs) {
                    if (rawVal->Op == "+") {
                        if (type == TypeID::Int) {
                            return this->Builder->CreateAdd(
                                GeneratorGlobalValue(rawVal->Lhs, type,std::move(vTable)),
                                GeneratorGlobalValue(rawVal->Rhs, type,std::move(vTable)));
                        } else {
                            return this->Builder->CreateFAdd(
                                GeneratorGlobalValue(rawVal->Lhs, type,std::move(vTable)),
                                GeneratorGlobalValue(rawVal->Rhs, type,std::move(vTable)));
                        }
                    } else if (rawVal->Op == "-") {
                        if (type == TypeID::Int) {
                            return this->Builder->CreateSub(
                                GeneratorGlobalValue(rawVal->Lhs, type,std::move(vTable)),
                                GeneratorGlobalValue(rawVal->Rhs, type,std::move(vTable)));
                        } else {
                            return this->Builder->CreateFSub(
                                GeneratorGlobalValue(rawVal->Lhs, type,std::move(vTable)),
                                GeneratorGlobalValue(rawVal->Rhs, type,std::move(vTable)));
                        }
                    } else if (rawVal->Op == "*") {
                        if (type == TypeID::Int) {
                            return this->Builder->CreateMul(
                                GeneratorGlobalValue(rawVal->Lhs, type,std::move(vTable)),
                                GeneratorGlobalValue(rawVal->Rhs, type,std::move(vTable)));
                        } else {
                            return this->Builder->CreateFMul(
                                GeneratorGlobalValue(rawVal->Lhs, type,std::move(vTable)),
                                GeneratorGlobalValue(rawVal->Rhs, type,std::move(vTable)));
                        }
                    } else if (rawVal->Op == "/") {
                        if (type == TypeID::Int) {
                            return this->Builder->CreateSDiv(
                                GeneratorGlobalValue(rawVal->Lhs, type,std::move(vTable)),
                                GeneratorGlobalValue(rawVal->Rhs, type,std::move(vTable)));
                        } else {
                            return this->Builder->CreateFDiv(
                                GeneratorGlobalValue(rawVal->Lhs, type,std::move(vTable)),
                                GeneratorGlobalValue(rawVal->Rhs, type,std::move(vTable)));
                        }
                    }
                } else if (rawVal->Lhs) {
                    return GeneratorGlobalValue(rawVal->Lhs, type,std::move(vTable));
                } else {
                    return nullptr;
                }
            }
        }

    }  // namespace codegen
}  // namespace otter
