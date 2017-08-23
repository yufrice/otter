#ifndef _OTTER_AST_AST_HPP_
#define _OTTER_AST_AST_HPP_

#include <string>
#include <vector>
#include <type_traits>
#include <memory>
#include <variant>
#include <iostream>

namespace otter {
    namespace ast {
        enum struct AstID {
            BaseID,
            FunctionID,
            FuncCallID,
            TypeID,
            ModuleID,
            NumberID,
            StringID,
            BoolID,
            IdentifierID,
            ExprID,
            MonoExprID,
            BinExprID,
            BindID,
            BuiltinID,
            StatementsID,
            IfStatementID,
            WhileStatementID,
        };

        enum struct TypeID { Unit, Int, Double, String, Bool, Nil };
        decltype(auto) getType = [](TypeID type){
            if(type == TypeID::Unit){
                return "Unit";
            }else if(type == TypeID::Int){
                return "Int";
            }else if(type == TypeID::Double){
                return "Double";
            }else if(type == TypeID::String){
                return "String";
            }else if(type == TypeID::Bool){
                return "Bool";
            }else if(type == TypeID::Nil){
                return "Nil";
            }
        };

        struct baseAST {
           private:
            AstID ID;

           public:
            baseAST(AstID id) : ID(id) {}
            virtual ~baseAST() {}
            inline auto getID() const -> AstID { return this->ID; }
        };

        struct identifierAST : public baseAST {
            std::string Ident;

            identifierAST(std::string ident)
                : baseAST(AstID::IdentifierID), Ident(ident){};
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

            numberAST(double val,TypeID type) : baseAST(AstID::NumberID), Val(val),Type(type){};
            static inline bool classof(baseAST const* base) {
                return base->getID() == AstID::NumberID;
            }

            decltype(auto) getVal() {
                std::variant<int,double> val;
                if(Type == TypeID::Int){
                    return val = (int)Val;
                }else if(Type == TypeID::Double){
                    return val = Val;
                }
            }
        };

        struct binaryExprAST : public baseAST {
            std::string Op;
            std::shared_ptr<baseAST> Lhs;
            std::shared_ptr<baseAST> Rhs;

            binaryExprAST(std::shared_ptr<baseAST>& lhs,std::string op, std::shared_ptr<baseAST>& rhs)
                : baseAST(AstID::BinExprID), Lhs(lhs),Rhs(rhs),Op(op) {}
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

        struct variableAST : public baseAST {
            std::string Name;
            TypeID Type;
            std::shared_ptr<baseAST> Val;

            variableAST(std::string name)
                : baseAST(AstID::BindID), Name(name){};
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

            functionAST(auto& null = nullptr) : baseAST(AstID::FunctionID){
            };
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

            void addAst(const auto& ast) {
                Statements.emplace_back(std::move(ast));
            }

            auto getStatements() -> std::vector<std::shared_ptr<baseAST>>& {
                return this->Statements;
            }
        };

        struct funcCallAST : public baseAST {
            std::string Name;
            std::vector<std::shared_ptr<baseAST>> Args;

            funcCallAST(std::string name)
                : baseAST(AstID::FuncCallID), Name(name){};
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

            void addAst(const auto& ast) {
                Stmt.emplace_back(ast);
            }
        };

    }  // namespace ast
}  // namespace otter

#endif
