#ifndef _OTTER_PARSER_HPP_
#define _OTTER_PARSER_HPP_

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
        x3::rule<class function, functionAST> const function("function");
        x3::rule<class variable, std::shared_ptr<variableAST>> const variable("variable");
        x3::rule<class statement, statementsAST> const statement("statement");
        x3::rule<class expr, std::shared_ptr<numberAST>> const expr("expr");
        // x3::rule<class type, TypeID> const type("type");
        x3::rule<class test, std::shared_ptr<int>> const test("test");

        auto const id_def =
            x3::lexeme[
            ((x3::alpha | '_') >> *(x3::alnum | '_'))
            ];

        auto const expr_def = x3::double_[detail::sharedAssign<ast::moduleAST>()];

        auto const statement_def = id;
        auto const variable_def = "let" >> id[detail::sharedAssign<ast::variableAST>()];
        auto const function_def = "(" >> *statement >> ")";
        // auto const module_def = *variable >> *function;
        auto const module_def = *test[detail::addAST()];
        auto const test_def = x3::int_[detail::sharedAssign<int>()];

        BOOST_SPIRIT_DEFINE(test,expr, statement, variable, function, module, id);


    } //namespace parser
} //namespace otter

#endif
