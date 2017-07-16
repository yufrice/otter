#ifndef _OTTER_SEMANTICS_PRECHECK_HPP_
#define _OTTER_SEMANTICS_PRECHECK_HPP_

namespace otter {
namespace semantics {

class preCheck {
   private:
    std::shared_ptr<ast::moduleAST> Module;

   public:
    preCheck(std::shared_ptr<ast::moduleAST> module) : Module(module) {}

    bool checkOverLap();
    bool checkType();
};

}
}  // namespace otter

#endif
