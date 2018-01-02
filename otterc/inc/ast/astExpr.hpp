#ifndef _OTTER_AST_ASTEXPR_HPP_
#define _OTTER_AST_ASTEXPR_HPP_

#include <ast/astBase.hpp>

namespace otter {
    namespace ast {

        struct binaryExprAST : public baseAST {
            std::string Op;
            std::shared_ptr<baseAST> Lhs;
            std::shared_ptr<baseAST> Rhs;

            binaryExprAST(std::shared_ptr<baseAST>& lhs,
                          std::string op,
                          std::shared_ptr<baseAST>& rhs)
                : baseAST(AstID::BinExprID), Lhs(lhs), Rhs(rhs), Op(op) {}
            static inline bool classof(baseAST const* base) {
                return base->getID() == AstID::BinExprID;
            }
        };

        struct monoExprAST : public baseAST {
            std::string Op;
            std::shared_ptr<baseAST> Lhs;

            monoExprAST(std::string op) : baseAST(AstID::MonoExprID), Op(op) {}
            monoExprAST(std::shared_ptr<baseAST>& ast)
                : baseAST(AstID::MonoExprID), Lhs(std::move(ast)) {}
            static inline bool classof(baseAST const* base) {
                return base->getID() == AstID::MonoExprID;
            }

            template <typename T>
            void addAst(const T& ast) {
                this->Lhs = std::move(ast);
            }

            void setVal(const std::string& str) { this->Op = str; }
        };

    }  // namespace ast
}  // namespace otter
#endif