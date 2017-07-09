#ifndef _OTTER_AST_AST_HPP_
#define _OTTER_AST_AST_HPP_

#include <string>
#include <vector>
#include <variant>
#include <memory>

namespace otter{
    namespace ast{
        enum struct AstID{
            BaseID,
            FunctionID,
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

        enum struct TypeID{
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

            identifierAST(std::string ident):baseAST(AstID::StringID),Ident(ident){};
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
        };

        struct numberAST : public baseAST {
            double Val;

            numberAST(double val):baseAST(AstID::NumberID),Val(val){};
            static inline bool classof(baseAST const *base){
                return base->getID() == AstID::NumberID;
            }

            auto getVals() -> double {
                return this->Val;
            }
        };

        struct binaryExprAST;
        struct binaryExprAST;
        struct monoExprAST;

        using exprType = std::variant<
            std::shared_ptr<identifierAST>,
            std::shared_ptr<numberAST>,
            std::shared_ptr<binaryExprAST>,
            std::shared_ptr<monoExprAST>
            >;
        struct binaryExprAST : public baseAST{
            std::string Op;
            exprType Lhs;
            exprType Rhs;

            binaryExprAST():baseAST(AstID::BinExprID){}
            static inline bool classof(baseAST const *base){
                return base->getID() == AstID::BinExprID;
            }

            void addAst(const auto& ast,auto& type){
                if(type == "lhs"){
                    this->Lhs = std::move(ast);
                }else if(type == "rhs"){
                    this->Rhs = std::move(ast);
                }
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
            exprType Lhs;

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

        using variableType = std::variant<std::shared_ptr<stringAST>>;
        struct variableAST : public baseAST {
            std::string Name;
            std::string Type;
            variableType Val;

            variableAST(std::string name):baseAST(AstID::BindID),Name(name){};
            static inline bool classof(baseAST const *base){
                return base->getID() == AstID::BindID;
            }

            template<typename T>
            void addAst(const T& ast){
                this->Val = std::move(ast);
            }

            void setVal(const std::string& str){
                    this->Type = str;
            }

            auto getName() -> std::string& {
                return this->Name;
            }

            auto getType() -> std::string& {
                return this->Type;
            }

            auto getVal() -> variableType& {
                return this->Val;
            }

        };

        struct statementsAST : public baseAST{
            std::string statement;

            statementsAST():baseAST(AstID::StatementsID){};
            static inline bool classof(baseAST const *base){
                return base->getID() == AstID::StatementsID;
            }
        };
        struct functionAST : public baseAST{
            std::vector<std::shared_ptr<statementsAST>> statements;

            functionAST():baseAST(AstID::FunctionID){};
            static inline bool classof(baseAST const *base) {
                return base->getID() == AstID::FunctionID;
            }

            auto getStatements() -> std::vector<std::shared_ptr<statementsAST>>& {
                return this->statements;
            }
        };

        struct moduleAST : public baseAST{
            std::vector<std::shared_ptr<variableAST>> Vars;
            std::vector<std::shared_ptr<functionAST>> Funcs;

            moduleAST():baseAST(AstID::ModuleID){};
            static inline bool classof(baseAST const *base) {
                return base->getID() == AstID::ModuleID;
            }

            void addAst(const auto& ast,auto& type){
                if(type == typeid(variableAST)){
                    Vars.push_back(std::move(ast));
                }else if(type == typeid(functionAST)){
                    // Funcs.push_back(std::move(ast));
                }
            }
            auto getFuncs() -> std::vector<std::shared_ptr<functionAST>>& {
                return this-> Funcs;
            }

            auto getVars() -> std::vector<std::shared_ptr<variableAST>>& {
                return this->Vars;
            }
        };



    } //namespace ast
} //namespace otter


#endif
