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

            numberAST():baseAST(AstID::NumberID){};
            static inline bool classof(baseAST const *base){
                return base->getID() == AstID::NumberID;
            }

            auto getVals() -> double {
                return this->Val;
            }
        };

        struct variableAST : public baseAST {
            std::string Name;
            // TypeID Type;
            // std::shared_ptr<baseAST> Val;

            variableAST(std::string name):baseAST(AstID::BindID),Name(name){};
            static inline bool classof(baseAST const *base){
                return base->getID() == AstID::BindID;
            }

            auto getName() -> std::string& {
                return this->Name;
            }

            // auto getType() -> TypeID& {
            //     return this->Type;
            // }

            // auto getVal() -> baseAST& {
            //     return this->Val;
            // }

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
