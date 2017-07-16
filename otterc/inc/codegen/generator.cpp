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
            throw std::string("type error");
        }
    }

    Module->dump();
    return this->Module;
}

GlobalVariable* Generator::generateGlovalVariable(
    std::shared_ptr<variableAST> var) {
    if (detail::sharedIsa<functionAST>(var->Val)) {
        std::cout << "func" << std::endl;
    } else if (detail::sharedIsa<stringAST>(var->Val)) {
        if (TypeID::String == var->Type) {
            return GeneratorGlobalString(var);
        }
    } else {
        std::cout << "value" << std::endl;
    }
}

GlobalVariable* Generator::GeneratorGlobalString(
    std::shared_ptr<variableAST> var) {
    if (detail::sharedIsa<stringAST>(var->Val)) {
        if (auto rawStr = detail::sharedCast<stringAST>(var->Val)) {
            ArrayType* Type = ArrayType::get(
                IntegerType::get(this->TheContext, 8), (rawStr->Str).length());
            auto Name = var->Name;
            GlobalVariable* gvar =
                new GlobalVariable(*this->Module, Type, true,
                                   GlobalValue::PrivateLinkage, 0, Name);

            Constant* strCons =
                ConstantDataArray::getString(this->TheContext, rawStr->Str);
            gvar->setInitializer(strCons);
            return gvar;
        }
    }
    throw std::string("no viable onversion from 'Digit' to 'String'\n\t" +
                      var->Name);
}

}  // namespace codegen
}  // namespace otter
