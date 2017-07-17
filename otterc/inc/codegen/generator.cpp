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
            for (auto var : mod->Vars) {
                if (!generateGlovalVariable(var)) {
                    // throw std::string("type error");
                }
            }

            std::cout << "\n######################" << std::endl;
            Module->dump();
            return this->Module;
        }

        GlobalVariable* Generator::generateGlovalVariable(
            std::shared_ptr<variableAST> var) {
            if (detail::sharedIsa<functionAST>(var->Val)) {
                std::cout << "func" << std::endl;
            } else if (detail::sharedIsa<stringAST>(var->Val)) {
                if (TypeID::String == var->Type) {
                    return this->GeneratorGlobalString(var);
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
                    this->Builder->CreateStore(gvar, value);
                    return gvar;
                } else {
                    return nullptr;
                }
            }
        }

        GlobalVariable* Generator::GeneratorGlobalString(
            std::shared_ptr<variableAST> var) {
            if (auto rawStr = detail::sharedCast<stringAST>(var->Val)) {
                ArrayType* Type =
                    ArrayType::get(IntegerType::get(this->TheContext, 8),
                                   (rawStr->Str).length());
                auto Name = var->Name;
                GlobalVariable* gvar =
                    new GlobalVariable(*this->Module, Type, true,
                                       GlobalValue::CommonLinkage, 0, Name);

                Constant* strCons =
                    ConstantDataArray::getString(this->TheContext, rawStr->Str);
                gvar->setInitializer(strCons);
                return gvar;
            }
        }

        Function* Generator::GeneratorFunction(std::shared_ptr<baseAST> var) {}

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
                            GeneratorValue(rawVal->Rhs, valueType), "add");
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
