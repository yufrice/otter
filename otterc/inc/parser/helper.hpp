#ifndef OTTER_HELPER_HPP
#define OTTER_HELPER_HPP

#include <boost/spirit/home/x3.hpp>
#include "ast.hpp"

namespace otter{
    namespace parser{
        namespace detail{
            using namespace boost::spirit;
            inline auto assign(){
                return [](auto& ctx){
                    x3::_val(ctx) = x3::_attr(ctx);
                };
            }

            inline auto numberAssign(){
                return [](auto& ctx){
                    x3::_val(ctx) = new ast::intNumberAST(_attr(ctx));
                };
            }

            inline auto addAssign(){
                return [](auto& ctx){
                    x3::_val(ctx)->push(x3::_attr(ctx))
                }
            }

            template<typename T>
                auto assign(T&& t){
                    return std::bind(
                            [](auto& ctx,auto&& arg){
                            x3::_val(ctx) = std::forward<decltype(arg)>(arg);
                            },
                            std::placeholders::_1,
                            std::forward<T>(t)
                            );
                }
        } // name space detail
    } // namespace parser
} //namespace otter

#endif
