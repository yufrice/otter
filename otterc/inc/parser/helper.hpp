#ifndef _OTTER_PARSER_HELPER_HPP_
#define _OTTER_PARSER_HELPER_HPP_

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

            inline auto assign(auto&& arg){
                return [&arg](auto& ctx,auto&& arg){
                        x3::_val(ctx)->setVal(arg);
                    };
            }

            template<typename T>
            decltype(auto) sharedAssign(){
                return [](auto& ctx){
                    x3::_val(ctx) = std::move(std::make_shared<T>(x3::_attr(ctx)));
                };
            }

            template<typename T>
            decltype(auto) sharedAssign(auto&& arg){
                return [&arg](auto& ctx){
                    x3::_val(ctx) = std::move(std::make_shared<T>(arg));
                };
            }


            decltype(auto) sharedAdd(){
                return [](auto& ctx){
                    x3::_val(ctx)->setVal(_attr(ctx));
                };
            }

            decltype(auto) addAST(){
                return [](auto& ctx){
                    x3::_val(ctx)->addAst(std::move(x3::_attr(ctx)));
                };
            }

            decltype(auto) addAST(auto&& arg){
                return [&arg](auto& ctx){
                    x3::_val(ctx)->addAst(std::move(x3::_attr(ctx)), arg);
                };
            }

            decltype(auto) type(){
                return [](auto& ctx){
                    std::cout << typeid(x3::_attr(ctx)).name()
                        << " : " << x3::_attr(ctx) << std::endl;
                };
            }
        } // name space detail
    } // namespace parser
} //namespace otter

#endif
