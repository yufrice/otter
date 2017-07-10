#ifndef _OTTER_PARSER_PARSER_HPP_
#define _OTTER_PARSER_PARSER_HPP_

#include <string>
#include <vector>
#include <boost/spirit/home/x3.hpp>
#include "helper.hpp"

namespace otter {
    namespace parser {
        using namespace boost::spirit;
        using namespace otter::ast;

        x3::rule<class id, std::string> const id("id");
        x3::rule<class modules, std::shared_ptr<moduleAST>> const module("module");
        x3::rule<class function, std::shared_ptr<functionAST>> const function("function");
        x3::rule<class variable, std::shared_ptr<variableAST>> const variable("variable");
        x3::rule<class statement, std::shared_ptr<statementsAST>> const statement("statement");

        x3::rule<class expr, exprType> const expr("expr");
        x3::rule<class notExpr, exprType> const notExpr("notExpr");
        x3::rule<class boolExpr, exprType> const boolExpr("boolExpr");
        x3::rule<class addExpr, exprType> const addExpr("addExpr");
        x3::rule<class mulExpr, exprType> const mulExpr("mulExpr");
        x3::rule<class number, exprType> const number("number");

        x3::rule<class type, TypeID> const type("type");
        x3::rule<class value, variableType> const value("value");
        x3::rule<class string, std::string> const string("string");

        auto const id_def =
            x3::lexeme[
            ((x3::alpha | '_') >> *(x3::alnum | '_'))
            ];

        auto const number_def = x3::int_[detail::sharedAssign<numberAST>()] |
            x3::double_[detail::sharedAssign<numberAST>()] ;
            // id |
            // '(' >> expr >> ')';
        auto const mulExpr_def = number[detail::sharedAssign<binaryExprAST>()] ;//>> *(
                //     ("*" >> number)
                //     |("/" >> number)
                // );
        auto const addExpr_def = mulExpr >> *(
                    ("+" >> mulExpr)
                    |("-" >> mulExpr)
                );
        auto const boolExpr_def = addExpr >> *(
                ("=" >> addExpr)
                |("<" >> addExpr)
                |(">" >> addExpr)
                |("<=" >> addExpr)
                |("=>" >> addExpr)
                |(">=" >> addExpr)
                );
        auto const notExpr_def = boolExpr | x3::lit("!")[detail::assign("!")] >> boolExpr;
        auto const expr_def = notExpr[detail::addAST("lhs")] >> *(
                (x3::lit("&&")[detail::assign("&&")] >> notExpr[detail::addAST("rhs")])
                |(x3::lit("||")[detail::assign("||")] >> notExpr[detail::addAST("rhs")])
                );


        auto const string_def = x3::lit('"') >> +((x3::char_)- x3::lit('"')) >> x3::lit('"');
        auto const value_def = string[detail::sharedAssign<stringAST>()] | number;// expr[detail::sharedAssign<binaryExprAST>()];

        auto const type_def = ":" >> (x3::lit("int")[detail::assign(TypeID::Int)]
                | x3::lit("double")[detail::assign(TypeID::Double)]
                | x3::lit("string")[detail::assign(TypeID::String)]);
        auto const variable_def = x3::lit("let") >> id[detail::sharedAssign<variableAST>()] >> type[detail:: sharedAdd()] >> "=" >>
            value[detail::addAST()];

        auto const statement_def = id[detail::sharedAssign<statementsAST>()] >> (id[detail::sharedAdd()] | string[detail::sharedAdd()]);
        auto const function_def = "let" >> id[detail::sharedAssign<functionAST>()] >> "=" >> *statement[detail::addAST()] >> ";";

        auto const module_def = *variable[detail::addAST(typeid(variableAST))] >> *function[detail::addAST()];

        BOOST_SPIRIT_DEFINE(
                value, string, type,
                expr, notExpr, boolExpr, addExpr, mulExpr, number,
                statement, variable, function, module, id);


    } //namespace parser
} //namespace otter

#endif
