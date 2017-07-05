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
            AstID getValueID() const {
                return this->ID;
            }
        };

        struct functionAST : public baseAST{
            int Val;
            functionAST():baseAST(AstID::FunctionID){};
        };
        struct moduleAST : public baseAST{
            std::vector<functionAST> Func;
            moduleAST():baseAST(AstID::ModuleID){};
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
