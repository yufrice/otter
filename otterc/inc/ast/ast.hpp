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

        struct stringAST : public baseAST {
            std::string Val;

            stringAST(std::string val):baseAST(AstID::StringID),Val(val){};
            static inline bool classof(baseAST const *base){
                return base->getID() == AstID::StringID;
            }
        };

        struct binaryExprAST : public baseAST{
            std::string Op;
            std::shared_ptr<baseAST> Lhs;
            std::shared_ptr<baseAST> Rhs;

            binaryExprAST():baseAST(AstID::BinExprID){}
            static inline bool classof(baseAST const *base){
                return base->getID() == AstID::BinExprID;
            }
        };

        struct monoExprAST : public baseAST{
            std::string Op;
            std::shared_ptr<baseAST> Lhs;

            monoExprAST():baseAST(AstID::MonoExprID){}
            static inline bool classof(baseAST const *base){
                return base->getID() == AstID::MonoExprID;
            }
        };

        struct variableAST : public baseAST {
            std::string Name;
            std::string Type;
            std::shared_ptr<baseAST> Val;

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

            auto getVal() -> std::shared_ptr<baseAST>& {
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
            std::vector<statementsAST> statements;

            functionAST():baseAST(AstID::FunctionID){};
            static inline bool classof(baseAST const *base) {
                return base->getID() == AstID::FunctionID;
            }

            auto getStatements() -> std::vector<statementsAST>& {
                return this->statements;
            }
        };

        struct moduleAST : public baseAST{
            std::vector<std::shared_ptr<variableAST>> Vars;
            // std::vector<functionAST> Funcs;
            // std::vector<std::shared_ptr<int>> Vars;

            moduleAST():baseAST(AstID::ModuleID){};
            static inline bool classof(baseAST const *base) {
                return base->getID() == AstID::ModuleID;
            }

            template<typename T>
            void addAst(const T& ast){
                if(bool(typeid(variableAST) == typeid(*ast))){
                    Vars.push_back(std::move(ast));
                }
            }
            // auto getFuncs() -> std::vector<functionAST>& {
            //     return this-> Funcs;
            // }
            //
            auto getVars() -> std::vector<std::shared_ptr<variableAST>>& {
                return this->Vars;
            }
        };



    } //namespace ast
} //namespace otter


#endif
