#ifndef OTTER_HELPER_HPP
#define OTTER_HELPER_HPP

#include <boost/spirit/home/x3.hpp>
#include "../ast/ast.hpp"

namespace otter{
    namespace parser{
        namespace detail{
            using namespace boost::spirit;
            inline auto assign(){
                return [](auto& ctx){
                    x3::_val(ctx) = x3::_attr(ctx);
                };
            }

            template<typename T>
            decltype(auto) sharedAssign(){
                return [](auto& ctx){
                    std::cout << typeid(_attr(ctx)).name();
                    // x3::_val(ctx) = std::move(std::make_shared<T>(x3::_attr(ctx)));
                };
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
