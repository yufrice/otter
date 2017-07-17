#ifndef _OTTER_PARSER_ON_ERROR_HPP_
#define _OTTER_PARSER_ON_ERROR_HPP_

#include <boost/spirit/home/x3.hpp>

namespace otter {
    namespace parser {

        struct moduleClass {
            template <typename Iterator, typename Exception, typename Context>
            boost::spirit::x3::error_handler_result on_error(
                Iterator&,
                Iterator const& last,
                Exception const& x,
                Context const& context) {
                std::cout << "error" << std::endl;
                return boost::spirit::x3::error_handler_result::fail;
            }
        };

        struct funcClass {
            template <typename Iterator, typename Exception, typename Context>
            boost::spirit::x3::error_handler_result on_error(
                Iterator&,
                Iterator const& last,
                Exception const& x,
                Context const& context) {
                std::cout << "error" << std::endl;
                return boost::spirit::x3::error_handler_result::fail;
            }
        };

    }  // namespace parser
}  // namespace otter

#endif
