#include <semantics/preCheck.hpp>

namespace otter {
    namespace semantics {
        void preCheck::check() {
            this->checkOverLap();
            this->checkType(this->Module->Stmt);
        }
        void preCheck::checkOverLap() {
            for (auto stmt : this->Module->Stmt) {
                if(auto var = std::dynamic_pointer_cast<ast::variableAST>(stmt)){
                    this->checkIdAssign(var->Val);
                    auto ptr = var->Val.get();
                    if (nameQueue.end() !=
                        std::find(nameQueue.begin(), nameQueue.end(), var->Name)) {
                            throw std::string("redeclaration of " + var->Name);
                    } else {
                        nameQueue.emplace_back(var->Name);
                    }
                }
            }
        }

        void preCheck::checkType(const std::vector<std::shared_ptr<ast::baseAST>> stmts) {
            for (auto stmt: stmts){
                if(auto var = std::dynamic_pointer_cast<ast::variableAST>(stmt)){

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
                        if (var->Type != rawPtr->Type) {
                            if(var->Type == ast::TypeID::Double){
                                throw std::string("invaild conversion 'Int' to 'Double' : " + var->Name);
                            }else{
                                throw std::string("invaild conversion 'Double' to 'Int' : " + var->Name);
                            }
                        }
                    } else if (auto rawPtr =
                               dynamic_cast<ast::binaryExprAST*>(ptr)) {
                        if ((var->Type != ast::TypeID::Double) &&
                            (var->Type != ast::TypeID::Int)) {
                            throw std::string(
                                "invalid conversion from 'Digit' to " + var->Name);
                        }
                    } else if (auto rawPtr = dynamic_cast<ast::functionAST*>(ptr)) {
                        this->checkType(rawPtr->Statements);
                    } 
                }
            }
        }

        void preCheck::checkIdAssign(auto& val) {
            auto ptr = val.get();
            if (auto rawPtr = dynamic_cast<ast::identifierAST*>(ptr)) {
                if (nameQueue.end() == std::find(nameQueue.begin(),
                                                 nameQueue.end(),
                                                 rawPtr->Ident)) {
                    throw std::string(rawPtr->Ident + " was not declar");
                }
            } else if (auto rawPtr = dynamic_cast<ast::binaryExprAST*>(ptr)) {
                if (rawPtr->Rhs) {
                    checkIdAssign(rawPtr->Rhs);
                    checkIdAssign(rawPtr->Lhs);
                } else {
                    checkIdAssign(rawPtr->Lhs);
                }
            }
        }

    }  // namespace semantics
}  // namespace otter
