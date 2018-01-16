#ifndef _OTTER_AST_ASTSTMT_HPP_
#define _OTTER_AST_ASTSTMT_HPP_

#include <ast/astBase.hpp>

namespace otter {
    namespace ast {

        struct variableAST : public baseAST {
            std::string Name;
            TypeID Type;
            std::shared_ptr<baseAST> Val;

            variableAST(std::string name)
                : baseAST(AstID::BindID), Name(name){
                    logger._assert("Build VariableAST : " + name);
               };
            static inline bool classof(baseAST const* base) {
                return base->getID() == AstID::BindID;
            }

            template <typename T>
            void addAst(const T& ast) {
                this->Val = std::move(ast);
            }

            void setVal(const TypeID& type) { this->Type = type; }

            auto getName() -> std::string& { return this->Name; }

            auto getType() -> TypeID& { return this->Type; }

            auto getVal() -> std::shared_ptr<baseAST>& { return this->Val; }
        };

        struct statementsAST : public baseAST {
            std::string Statement;
            std::string Arg;

            statementsAST(std::string stt)
                : baseAST(AstID::StatementsID), Statement(stt){};
            static inline bool classof(baseAST const* base) {
                return base->getID() == AstID::StatementsID;
            }

            void setVal(const std::string& arg) { this->Arg = arg; }
        };

        struct functionAST : public baseAST {
            std::vector<std::string> Args;
            std::vector<TypeID> Types;
            std::vector<std::shared_ptr<baseAST>> Statements;
            bool flag;

            functionAST(auto& null = nullptr)
                : baseAST(AstID::FunctionID), flag(false){};
            static inline bool classof(baseAST const* base) {
                return base->getID() == AstID::FunctionID;
            }

            template <typename T>
            auto setVal(T Arg) ->
                typename std::enable_if<!std::is_enum<T>::value>::type {
                Args.emplace_back(Arg);
            }

            template <typename T>
            auto setVal(T Type) ->
                typename std::enable_if<std::is_enum<T>::value>::type {
                Types.emplace_back(Type);
            }

            auto setVal(bool Flag) { flag = Flag; }

            void addAst(const auto& ast) {
                Statements.emplace_back(std::move(ast));
            }
        };

        struct ifStatementAST : public baseAST {
            std::shared_ptr<baseAST> Cond;
            std::shared_ptr<baseAST> thenStmt;
            std::shared_ptr<baseAST> falseStmt;

            ifStatementAST(std::shared_ptr<baseAST> cond)
                : baseAST(AstID::IfStatementID),
                  Cond(cond),
                  thenStmt(nullptr){};

            ifStatementAST(std::string cond)
                : baseAST(AstID::IfStatementID), thenStmt(nullptr) {
                Cond = std::make_shared<identifierAST>(cond);
            };
            static inline bool classof(baseAST const* base) {
                return base->getID() == AstID::IfStatementID;
            }

            void setVal(const auto& ast) {
                if (thenStmt == nullptr) {
                    thenStmt = std::move(ast);
                } else {
                    falseStmt = std::move(ast);
                }
            }
        };


        struct funcCallAST : public baseAST {
            std::string Name;
            std::vector<std::shared_ptr<baseAST>> Args;
            bool reserved;

            funcCallAST(std::string name)
                : baseAST(AstID::FuncCallID), Name(name), reserved(false){
                    logger._assert("Build funcCallAST : " + name);
                    const std::vector<std::string> buildin{
                        "print",
                        "car",
                        "cdr",
                        "cons",
                        "eq", "neq",
                        "=",    "<>",
                        ">", "<", ">=", "<=",
                    };
                    if(buildin.end() != std::find(
                        buildin.begin(),buildin.end(),
                        name
                    )){
                        this->reserved = true;
                        logger._assert("Buildin found");
                    }
                };
            static inline bool classof(baseAST const* base) {
                return base->getID() == AstID::FuncCallID;
            }

            void addAst(const auto& ast) { Args.emplace_back(std::move(ast)); }
        };

        struct moduleAST : public baseAST {
            std::vector<std::shared_ptr<baseAST>> Stmt;

            moduleAST() : baseAST(AstID::ModuleID){};
            static inline bool classof(baseAST const* base) {
                return base->getID() == AstID::ModuleID;
            }

            void addAst(const auto& ast) { Stmt.emplace_back(ast); }
        };
    }  // namespace ast
}  // namespace otter
#endif