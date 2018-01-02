#ifndef _OTTER_AST_ASTBASE_HPP_
#define _OTTER_AST_ASTBASE_HPP_

#include <string>
#include <vector>
#include <memory>

#include <logger/logger.hpp>

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
            ListID,
            MonoExprID,
            BinExprID,
            BindID,
            BuiltinID,
            StatementsID,
            IfStatementID,
            WhileStatementID,
        };

        enum struct TypeID { Unit, Int, Double, String, Bool, Nil };
        decltype(auto) getType = [](TypeID type) {
            if (type == TypeID::Unit) {
                return "Unit";
            } else if (type == TypeID::Int) {
                return "Int";
            } else if (type == TypeID::Double) {
                return "Double";
            } else if (type == TypeID::String) {
                return "String";
            } else if (type == TypeID::Bool) {
                return "Bool";
            } else if (type == TypeID::Nil) {
                return "Nil";
            }
        };

        struct baseAST {
           private:
            AstID ID;

           public:
            logger::Logger logger;
            baseAST(AstID id) : ID(id) {};
            virtual ~baseAST() {}
            inline auto getID() const -> AstID { return this->ID; }
        };

    }  // namespace ast
}  // namespace otter
#endif