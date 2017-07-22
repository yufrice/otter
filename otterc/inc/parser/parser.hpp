#ifndef _OTTER_PARSER_PARSER_HPP_
#define _OTTER_PARSER_PARSER_HPP_

#include <string>
#include <vector>
#include <boost/spirit/home/x3.hpp>
#include "on_error.hpp"
#include "helper.hpp"

namespace otter {
    namespace parser {
        using namespace boost::spirit;
        using namespace otter::ast;

        x3::rule<class id, std::string> const id("id");
        x3::rule<moduleClass, std::shared_ptr<moduleAST>> const module(
            "module");
        x3::rule<funcClass, std::shared_ptr<functionAST>> const function(
            "function");
        x3::rule<class variable, std::shared_ptr<variableAST>> const variable(
            "variable");
        x3::rule<class statement, std::shared_ptr<statementsAST>> const
            statement("statement");

        x3::rule<class funcCall, std::shared_ptr<funcCallAST>> const funcCall(
            "funcCall");
        x3::rule<class lambda, std::shared_ptr<functionAST>> const lambda(
            "lambda");

        x3::rule<class expr, std::shared_ptr<baseAST>> const expr("expr");
        x3::rule<class notExpr, std::shared_ptr<baseAST>> const notExpr(
            "notExpr");
        x3::rule<class boolExpr, std::shared_ptr<baseAST>> const boolExpr(
            "boolExpr");
        x3::rule<class addExpr, std::shared_ptr<binaryExprAST>> const addExpr(
            "addExpr");
        x3::rule<class mulExpr, std::shared_ptr<binaryExprAST>> const mulExpr(
            "mulExpr");
        x3::rule<class number, std::shared_ptr<baseAST>> const number("number");

        x3::rule<class type, TypeID> const type("type");
        x3::rule<class value, std::shared_ptr<baseAST>> const value("value");
        x3::rule<class string, std::string> const string("string");

        auto const id_def =
            x3::lexeme[((x3::alpha | '_') >> *(x3::alnum | '_'))];

        auto const number_def = id[detail::sharedAssign<identifierAST>()] |
                                x3::double_[detail::sharedAssign<numberAST>()] |
                                x3::int_[detail::sharedAssign<numberAST>()] |
                                '(' >> addExpr[detail::assign()] >> ')';
        auto const mulExpr_def =
            number[detail::sharedAssign<binaryExprAST>()] >>
            *((x3::lit("*")[detail::sharedAdd("*")] >>
               number[detail::addAST()]) |
              (x3::lit("/")[detail::sharedAdd("/")] >>
               number[detail::addAST()]));
        auto const addExpr_def = mulExpr[detail::assign()] >>
                                 *((x3::lit("+")[detail::sharedAdd("+")] >>
                                    mulExpr[detail::addAST()]) |
                                   (x3::lit("-")[detail::sharedAdd("-")] >>
                                    mulExpr[detail::addAST()]));
        auto const boolExpr_def = addExpr >>
                                  *(("=" >> addExpr) | ("<" >> addExpr) |
                                    (">" >> addExpr) |
                                    ("<=" >> addExpr) |
                                    ("=>" >> addExpr) |
                                    (">=" >> addExpr));
        auto const notExpr_def =
            boolExpr | x3::lit("!")[detail::assign("!")] >> boolExpr;
        auto const expr_def = notExpr[detail::addAST("lhs")] >>
                              *((x3::lit("&&")[detail::assign("&&")] >>
                                 notExpr[detail::addAST("rhs")]) |
                                (x3::lit("||")[detail::assign("||")] >>
                                 notExpr[detail::addAST("rhs")]));

        auto const function_def =
            x3::lit("[](")[detail::sharedAssign<functionAST>(nullptr)] >>
            *(id[detail::sharedAdd()] >> type[detail::sharedAdd()]) >>
            x3::lit(')') >> +number[detail::addAST()] >> x3::lit(";");
        auto const string_def = x3::lit('"') >>
                                +((x3::char_)-x3::lit('"')) >> x3::lit('"');
        auto const value_def = string[detail::sharedAssign<stringAST>()];

        auto const type_def =
            ":" >> (x3::lit("int")[detail::assign(TypeID::Int)] |
                    x3::lit("double")[detail::assign(TypeID::Double)] |
                    x3::lit("string")[detail::assign(TypeID::String)] |
                    x3::lit("unit")[detail::assign(TypeID::Unit)]);

        auto const variable_def = x3::lit("let") >>
                                  id[detail::sharedAssign<variableAST>()] >>
                                  type[detail::sharedAdd()] >> "=" >>
                                  (value[detail::addAST()] |
                                   addExpr[detail::addAST()] |
                                   function[detail::addAST()]);

        auto const statement_def = id[detail::sharedAssign<statementsAST>()] >>
                                   (id[detail::sharedAdd()]);
        auto const funcCall_def = id[detail::sharedAssign<funcCallAST>()] >>
                                  x3::lit('(') >>
                                  *(id[detail::addAST()]) >> x3::lit(')');

        auto const module_def =
            *(variable[detail::addAST(typeid(variableAST))] |
              funcCall[detail::addAST()]);

        auto const skkiper =
            x3::space | x3::standard_wide::space |
            x3::lexeme["//" >> *(x3::char_ - x3::eol) >> x3::eol];

        BOOST_SPIRIT_DEFINE(value,
                            string,
                            type,
                            funcCall,
                            expr,
                            notExpr,
                            boolExpr,
                            addExpr,
                            mulExpr,
                            number,
                            statement,
                            variable,
                            function,
                            module,
                            id);

    }  // namespace parser
}  // namespace otter

#endif
