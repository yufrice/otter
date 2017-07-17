#include "preCheck.hpp"

namespace otter {
    namespace semantics {
        void preCheck::check() {
            this->checkOverLap();
            this->checkType();
        }
        void preCheck::checkOverLap() {
            for (auto var : this->Module->Vars) {
                auto ptr = var->Val.get();
                if (auto rawPtr = dynamic_cast<ast::identifierAST*>(ptr)) {
                    this->checkIdAssign(rawPtr);
                }
                if (nameQueue.end() !=
                    std::find(nameQueue.begin(), nameQueue.end(), var->Name)) {
                    throw std::string("redeclaration of " + var->Name);
                } else {
                    nameQueue.emplace_back(var->Name);
                }
            }
        }
        void preCheck::checkType() {
            for (auto var : this->Module->Vars) {
                auto ptr = var->Val.get();
                if (auto rawPtr = dynamic_cast<ast::stringAST*>(ptr)) {
                    if (var->Type != ast::TypeID::String) {
                        throw std::string(
                            "invalid conversion from 'String' to " + var->Name);
                    }
                } else if (auto rawPtr = dynamic_cast<ast::numberAST*>(ptr)) {
                    if ((var->Type != ast::TypeID::Double) &&
                        (var->Type != ast::TypeID::Int)) {
                        throw std::string(
                            "invalid conversion from 'Digit' to " + var->Name);
                    }
                    if (var->Type == ast::TypeID::Int) {
                        if (0 < rawPtr->Val - (int)rawPtr->Val) {
                            std::cout << "warning : " << var->Name
                                      << " is Double" << std::endl;
                        }
                    }
                } else if (auto rawPtr =
                               dynamic_cast<ast::binaryExprAST*>(ptr)) {
                    if ((var->Type != ast::TypeID::Double) &&
                        (var->Type != ast::TypeID::Int)) {
                        throw std::string(
                            "invalid conversion from 'Digit' to " + var->Name);
                    }
                }
            }
        }

        void preCheck::checkIdAssign(auto& rawPtr) {
            if (nameQueue.end() ==
                std::find(nameQueue.begin(), nameQueue.end(), rawPtr->Ident)) {
                throw std::string(rawPtr->Ident + " was not declar");
            }
        }

    }  // namespace semantics
}  // namespace otter
