#ifndef _OTTER_CONTEXT_CONTEXT_HPP_
#define _OTTER_CONTEXT_CONTEXT_HPP_


#include <llvm/IR/Module.h>
#include <memory>

namespace otter{
    namespace context{
        class Context {
            private:

            public:
                Context(std::shared_ptr<llvm::Module> module,
                    const std::string str):
                        Module(module),OutputPath(str){};
                ~Context(){};
                std::shared_ptr<llvm::Module> Module;
                const std::string OutputPath;
        };


    }  // namespace context
}  // namespace otter

#endif