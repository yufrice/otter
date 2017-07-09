#ifndef _OTTER_AST_ADAPT_HPP_
#define _OTTER_AST_ADAPT_HPP_

#include <boost/fusion/include/adapt_struct.hpp>
BOOST_FUSION_ADAPT_STRUCT(
        otter::ast::moduleAST,
        Vars,
        Funcs
        )



#endif
