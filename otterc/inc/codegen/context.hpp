#ifndef _OTTER_CODEGEN_CONTEXT_HPP_
#define _OTTER_CODEGEN_CONTEXT_HPP_

#include <llvm/IR/LLVMContext.h>

namespace otter{
    namespace codegen{
        class Context {
            private:
                struct FormatCounter{
                    bool digit;
                    bool real;
                    bool string;

                    FormatCounter():digit(false),real(false),string(false){};
                } 
                FormatCount;
                static llvm::LLVMContext llvmContext;

            public:
                Context(){};
                ~Context(){};
                llvm::LLVMContext& get(){return this->llvmContext;}
                bool resolveFormat(std::string);


        };


    }  // namespace codegen
}  // namespace otter

#endif