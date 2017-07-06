#ifndef _OTTER_AST_H_
#define _OTTER_AST_H_

#include <string>
#include <vector>
#include <boost/fusion/include/adapt_struct.hpp>

namespace otter{
    namespace ast{
        enum struct AstID{
            BaseID,
            FunctionID,
            ModuleID,
            IntNumberID,
            DoubleNumberID,
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

        class baseAST{
            private:
            AstID ID;
            public:
            baseAST(AstID id):ID(id){}
            virtual ~baseAST(){}
            inline auto getId() const -> AstID {
                return this->ID;
            }
        };

        struct statementsAST : public baseAST{
            statementsAST():baseAST(AstID::StatementsID){};
            static inline bool classof(baseAST const base){}
                return base->getValueID() == AstID::StatementsID;
            }
        };
        struct functionAST : public baseAST{
            std::vector<statementsAST> statements;
            functionAST():baseAST(AstID::FunctionID){};
            static inline bool classof(baseAST const base){}
                return base->getValueID() == AstID::FunctionID;
            }
            auto getStatements
        };

        struct 
        struct moduleAST : public baseAST{
            std::vector<global
            std::vector<functionAST> Funcs;
            moduleAST():baseAST(AstID::ModuleID){};
            static inline bool classof(baseAST const base){}
                return base->getValueID() == AstID::moduleAST;
            }

            auto getFuncs() -> std::vector<functionAST>& {
                return this-> Funcs;
            }
        };



    } //namespace ast
} //namespace otter


BOOST_FUSION_ADAPT_STRUCT(
        otter::ast::moduleAST,
        Func
        )

BOOST_FUSION_ADAPT_STRUCT(
        otter::ast::functionAST,
        Val
        )

#endif
