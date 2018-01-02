#ifndef _OTTER_AST_ASTVALUE_HPP_
#define _OTTER_AST_ASTVALUE_HPP_

#include <ast/astBase.hpp>
#include <variant>
#include <type_traits>

namespace otter {
    namespace ast {

        struct identifierAST : public baseAST {
            std::string Ident;

            identifierAST(std::string ident)
                : baseAST(AstID::IdentifierID), Ident(ident){
                    logger._assert("Build IdentifireAST : " + ident);
                };
            static inline bool classof(baseAST const* base) {
                return base->getID() == AstID::IdentifierID;
            }
        };

        struct stringAST : public baseAST {
            std::string Str;

            stringAST(std::string str) : baseAST(AstID::StringID), Str(str){};
            static inline bool classof(baseAST const* base) {
                return base->getID() == AstID::StringID;
            }

            auto getVal() -> std::string& { return this->Str; }
        };

        struct boolAST : public baseAST {
            bool Bl;

            boolAST(bool bl) : baseAST(AstID::BoolID), Bl(bl){};
            static inline bool classof(baseAST const* base) {
                return base->getID() == AstID::BoolID;
            }
        };

        struct numberAST : public baseAST {
            double Val;
            TypeID Type;

            numberAST(double val, TypeID type)
                : baseAST(AstID::NumberID), Val(val), Type(type){};
            static inline bool classof(baseAST const* base) {
                return base->getID() == AstID::NumberID;
            }

            decltype(auto) getVal() {
                std::variant<int, double> val;
                if (Type == TypeID::Int) {
                    return val = (int)Val;
                } else if (Type == TypeID::Double) {
                    return val = Val;
                }
            }
        };


        struct listAST : public baseAST {
            std::shared_ptr<baseAST> Car;
            std::shared_ptr<baseAST> Cdr;

            listAST(std::shared_ptr<baseAST> car)
                : baseAST(AstID::ListID), Car(car) {
                    logger._assert("Build ListAST");
                }
            auto setVal(auto cdr) { Cdr = cdr; }
            static inline bool classof(baseAST const* base) {
                return base->getID() == AstID::ListID;
            }
        };


    }  // namespace ast
}  // namespace otter
#endif