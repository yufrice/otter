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
                    x3::_val(ctx) = std::move(std::make_shared<T>(x3::_attr(ctx)));
                };
            }

            decltype(auto) addAST(){
                return [](auto& ctx){
                    x3::_val(ctx)->addAst(std::move(x3::_attr(ctx)));
                };
            }

            decltype(auto) type(){
                return [](auto& ctx){
                    std::cout << typeid(x3::_attr(ctx)).name() << std::endl;
                };
            }

            auto typeAssign(ast::TypeID id){
                return [](auto& ctx, ast::TypeID id){
                    x3::_val(ctx) = id;
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
