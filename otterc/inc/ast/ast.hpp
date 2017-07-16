#ifndef _OTTER_AST_AST_HPP_
#define _OTTER_AST_AST_HPP_

#include <string>
#include <vector>
#include <memory>
#include <iostream>

namespace otter{
    namespace ast{
        enum struct AstID{
            BaseID,
            FunctionID,
            FuncCallID,
            TypeID,
            ModuleID,
            NumberID,
            StringID,
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

        enum struct TypeID {
            Func,
            Int,
            Double,
            String,
            Nil
        };

        struct baseAST {
            private:
                AstID ID;
            public:
                baseAST(AstID id):ID(id){}
                virtual ~baseAST(){}
                inline auto getID() const -> AstID {
                    return this->ID;
                }
        };

        struct identifierAST : public baseAST {
            std::string Ident;

            identifierAST(std::string ident):baseAST(AstID::IdentifierID),Ident(ident){};
            static inline bool classof(baseAST const *base){
                return base->getID() == AstID::IdentifierID;
            }
        };

        struct stringAST : public baseAST {
            std::string Str;

            stringAST(std::string str):baseAST(AstID::StringID),Str(str){};
            static inline bool classof(baseAST const *base){
                return base->getID() == AstID::StringID;
            }

            auto getVal() -> std::string& {
                return this->Str;
            }
        };

        struct numberAST : public baseAST {
            double Val;

            numberAST(double val):baseAST(AstID::NumberID),Val(val){};
            static inline bool classof(baseAST const *base){
                return base->getID() == AstID::NumberID;
            }

            auto getVal() -> double {
                return this->Val;
            }
        };

        struct binaryExprAST : public baseAST{
            std::string Op;
            std::shared_ptr<baseAST> Lhs;
            std::shared_ptr<baseAST> Rhs;

            binaryExprAST(std::shared_ptr<baseAST>& ast):baseAST(AstID::BinExprID),Lhs(std::move(ast)){}
            static inline bool classof(baseAST const *base){
                return base->getID() == AstID::BinExprID;
            }

            void addAst(const auto& ast){
                    this->Rhs = std::move(ast);
            }

            template<typename T>
            void setLhs(const T& ast){
                this->Lhs = std::move(ast);
            }

            template<typename T>
            void setRhs(const T& ast){
                this->Rhs = std::move(ast);
            }

            void setVal(const std::string& op){
                this->Op = op;
            }
        };

        struct monoExprAST : public baseAST{
            std::string Op;
            std::shared_ptr<baseAST> Lhs;

            monoExprAST():baseAST(AstID::MonoExprID){}
            static inline bool classof(baseAST const *base){
                return base->getID() == AstID::MonoExprID;
            }

            template<typename T>
            void addAst(const T& ast){
                this->Lhs = std::move(ast);
            }

            void setVal(const std::string& str){
                this->Op = str;
            }
        };

        struct variableAST : public baseAST {
            std::string Name;
            TypeID Type;
            std::shared_ptr<baseAST> Val;

            variableAST(std::string name):baseAST(AstID::BindID),Name(name){};
            static inline bool classof(baseAST const *base){
                return base->getID() == AstID::BindID;
            }

            template<typename T>
            void addAst(const T& ast){
                this->Val = std::move(ast);
            }

            void setVal(const TypeID& type){
                this->Type = type;
            }

            auto getName() -> std::string& {
                return this->Name;
            }

            auto getType() -> TypeID& {
                return this->Type;
            }

            auto getVal() -> std::shared_ptr<baseAST>& {
                return this->Val;
            }

        };

        struct statementsAST : public baseAST{
            std::string Statement;
            std::string Arg;

            statementsAST(std::string stt):baseAST(AstID::StatementsID),Statement(stt){};
            static inline bool classof(baseAST const *base){
                return base->getID() == AstID::StatementsID;
            }

            void setVal(const std::string& arg){
                this->Arg = arg;
            }
        };

        struct functionAST : public baseAST{
            std::vector<std::string> Args;
            std::vector<std::shared_ptr<baseAST>> Statements;

            functionAST(decltype(nullptr) nl):baseAST(AstID::FunctionID){};
            static inline bool classof(baseAST const *base) {
                return base->getID() == AstID::FunctionID;
            }


            void setVal(const std::string& ast){
                Args.emplace_back(std::move(ast));
            }

            void addAst(const auto& ast){
                    Statements.emplace_back(std::move(ast));
            }


            auto getStatements() -> std::vector<std::shared_ptr<baseAST>>& {
                return this->Statements;
            }
        };

        struct funcCallAST : public baseAST{
            std::string Name;
            std::vector<variableAST> Args;

            funcCallAST(std::string name):baseAST(AstID::FuncCallID),Name(name){};
            static inline bool classof(baseAST const *base) {
                return base->getID() == AstID::FuncCallID;
            }

            void addAst(const auto& ast){
                    Args.emplace_back(std::move(ast));
            }
        };

        struct moduleAST : public baseAST{
            std::vector<std::shared_ptr<variableAST>> Vars;
            std::vector<std::shared_ptr<funcCallAST>> Funcs;

            moduleAST():baseAST(AstID::ModuleID){};
            static inline bool classof(baseAST const *base) {
                return base->getID() == AstID::ModuleID;
            }

            void addAst(const auto& ast,auto& type){
                if(std::is_convertible<decltype(ast), std::shared_ptr<variableAST>>::value == true){
                    Vars.push_back(std::move(ast));
                }
            }

            void addAst(const auto& ast){
                if(std::is_convertible<decltype(ast), std::shared_ptr<funcCallAST>>::value == true){
                    Funcs.push_back(std::move(ast));
                }
            }
            auto getFuncs() -> std::vector<std::shared_ptr<funcCallAST>>& {
                return this-> Funcs;
            }

            auto getVars() -> std::vector<std::shared_ptr<variableAST>>& {
                return this->Vars;
            }
        };



    } //namespace ast
} //namespace otter


#endif
