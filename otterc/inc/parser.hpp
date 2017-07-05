#ifndef OTTER_PARSER_HPP
#define OTTER_PARSER_HPP

#include <string>
#include <vector>
#include <boost/spirit/home/x3.hpp>
#include "ast.hpp"

namespace otter {
    namespace parser {
        using namespace boost::spirit;
        using namespace otter::ast;

        x3::rule<class id, std::string> const id("id");
        x3::rule<class modules, moduleAST> const module("module");
        x3::rule<class function, functionAST> const function("function");

        auto const function_def = x3::int_;
        auto const module_def = *function;
        auto const id_def =
            x3::lexeme[
            ((x3::alpha | '_') >> *(x3::alnum | '_'))
            ];

        BOOST_SPIRIT_DEFINE(function, module, id);


    } //namespace parser
} //namespace otter

#endif
