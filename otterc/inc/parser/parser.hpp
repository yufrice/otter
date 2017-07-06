#ifndef _OTTER_PARSER_HPP_
#define _OTTER_PARSER_HPP_

#include <string>
#include <vector>
#include <boost/spirit/home/x3.hpp>

namespace otter {
    namespace parser {
        using namespace boost::spirit;
        using namespace otter::ast;

        x3::rule<class id, std::string> const id("id");
        x3::rule<class modules, moduleAST> const module("module");
        x3::rule<class function, functionAST> const function("function");
        x3::rule<class variable, variableAST> const variable("variable");
        x3::rule<class statement, statementsAST> const statement("statement");
        x3::rule<class exp, numberAST> const expr("expr");
        // x3::rule<class type, TypeID> const type("type");

        auto const id_def =
            x3::lexeme[
            ((x3::alpha | '_') >> *(x3::alnum | '_'))
            ];

        auto const expr_def = x3::double_;

        auto const statement_def = id;
        // auto const variable_def = "let" >> id >> "::" >> expr;
        auto const variable_def = "let" >> id;
        auto const function_def = "(" >> *statement >> ")";
        auto const module_def = *variable >> *function;

        BOOST_SPIRIT_DEFINE(expr, statement, variable, function, module, id);


    } //namespace parser
} //namespace otter

#endif
