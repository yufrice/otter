#ifndef OTTER_HELPER_HPP
#define OTTER_HELPER_HPP

#include <boost/spirit/home/x3.hpp>
#include "../ast/ast.hpp"

namespace otter{
    namespace parser{
        namespace detail{
            using namespace boost::spirit;
            // inline auto assign(){
            //     return [](auto& ctx){
            //         x3::_val(ctx) = x3::_attr(ctx);
            //     };
            // }

            template<typename T>
            decltype(auto) sharedAssign(){
                return [](auto& ctx){
                    x3::_val(ctx) = std::move(std::make_shared<T>(x3::_attr(ctx)));
                };
            }

            auto assign(){
                return [](auto& ctx,auto&& arg){
                        x3::_val(ctx)->setVal(arg);
                    };
            }

            template<bool hs>
            decltype(auto) exprAssign(){
                return [](auto& ctx){
                    if(hs == true){
                        x3::_val(ctx)->setLhs(std::move(_attr(ctx)));
                    }else if(hs == false){
                        x3::_val(ctx)->setRhs(std::move(_attr(ctx)));
                    }
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

            decltype(auto) type(){
                return [](auto& ctx){
                    std::cout << typeid(x3::_attr(ctx)).name() << std::endl;
                };
            }
        } // name space detail
    } // namespace parser
} //namespace otter

#endif
