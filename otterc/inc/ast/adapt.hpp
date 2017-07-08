#ifndef _OTTER_AST_ADAPT_HPP_
#define _OTTER_AST_ADAPT_HPP_

#include <boost/fusion/include/adapt_struct.hpp>
// BOOST_FUSION_ADAPT_STRUCT(
//         otter::ast::moduleAST,
//         Vars
//         )

// BOOST_FUSION_ADAPT_STRUCT(
//         otter::ast::numberAST,
//         Val
//         )
//
BOOST_FUSION_ADAPT_STRUCT(
        otter::ast::variableAST,
        Name,
        Type
        )
//
// BOOST_FUSION_ADAPT_STRUCT(
//         otter::ast::statementsAST,
//         statement
//         )
//
// BOOST_FUSION_ADAPT_STRUCT(
//         otter::ast::moduleAST,
//         Vars,
//         Funcs
//         )
//
// BOOST_FUSION_ADAPT_STRUCT(
//         otter::ast::functionAST,
//         statements
//         )

#endif
