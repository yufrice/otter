#ifndef _OTTER_SEMANTICS_PRECHECK_HPP_
#define _OTTER_SEMANTICS_PRECHECK_HPP_

#include <memory>
#include <deque>
#include <algorithm>
#include <ast/ast.hpp>

namespace otter {
    namespace semantics {

        class preCheck {
           private:
            std::shared_ptr<ast::moduleAST> Module;
            std::deque<std::string> nameQueue;

           public:
            preCheck(std::shared_ptr<ast::moduleAST> module) : Module(module) {}

            void check();
            void checkOverLap();
            void checkType();
            void checkIdAssign(auto&);
        };

    }  // namespace semantics
}  // namespace otter

#endif
