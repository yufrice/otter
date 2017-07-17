#ifndef _OTTER_SEMANTICS_PRECHECK_HPP_
#define _OTTER_SEMANTICS_PRECHECK_HPP_

#include <memory>
#include "../ast/ast.hpp"

namespace otter {
    namespace semantics {

        class preCheck {
           private:
            std::shared_ptr<ast::moduleAST> Module;

           public:
            preCheck(std::shared_ptr<ast::moduleAST> module) : Module(module) {}

            bool check();
            bool checkOverLap();
            bool checkType();
        };

    }  // namespace semantics
}  // namespace otter

#endif
