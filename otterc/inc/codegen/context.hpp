#ifndef _OTTER_CODEGEN_CONTEXT_HPP_
#define _OTTER_CODEGEN_CONTEXT_HPP_

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>

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

                std::vector<llvm::Type*> argsTypes;
                bool currentFunc;
                static llvm::LLVMContext llvmContext;

            public:
                Context():currentFunc(false){};
                ~Context(){};
                llvm::LLVMContext& get(){return this->llvmContext;}
                bool resolveFormat(std::string);
                void setCFunc(const bool& f){this->currentFunc = f;}
                bool getCFunc(){return this->currentFunc;}

                void setType(llvm::Type*);
                llvm::Type* getType();
                bool clearType();
        };


    }  // namespace codegen
}  // namespace otter

#endif