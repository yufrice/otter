#include "generator.hpp"
#include <any>

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
        generateGlovalVariable(var);
    }
    Module->dump();
    return this->Module;
}

GlobalVariable* Generator::generateGlovalVariable(
    std::shared_ptr<variableAST> var) {
    auto rawVar = var->Val.get();
    if (TypeID::String == var->Type) {
        if (isa<stringAST>(rawVar)) {
            if (auto rawStr = dyn_cast<stringAST>(rawVar)) {
                ArrayType* Type =
                    ArrayType::get(IntegerType::get(this->TheContext, 32),
                                   (rawStr->Str).length());
                auto Name = var->Name;
                auto Gvar = new GlobalVariable(
                    *this->Module, Type, true,
                    GlobalVariable::LinkageTypes::CommonLinkage, nullptr, Name);
                Gvar->setAlignment(1);
                Constant* const_int64_6 = ConstantInt::get(
                    this->TheContext, APInt(64, StringRef("0"), 10));
                std::vector<Constant*> const_ptr_5v;
                const_ptr_5v.emplace_back(const_int64_6);
                const_ptr_5v.emplace_back(const_int64_6);
                Constant* const_ptr_5 =
                    ConstantExpr::getGetElementPtr(Type, Gvar, const_ptr_5v);
                Gvar->setInitializer(const_ptr_5);
            }
        }
    }
}
}  // namespace codegen
}  // namespace otter
