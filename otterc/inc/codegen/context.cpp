#include "context.hpp"

namespace otter{
    namespace codegen{

        llvm::LLVMContext Context::llvmContext;

        bool Context::resolveFormat(std::string format){
            if(format == "%d\n"){
                if(!this->FormatCount.digit){
                    this->FormatCount.digit = true;
                    return false;
                }else{
                    return true;
                }
            }else if(format == "%lf\n"){
                if(!this->FormatCount.real){
                    this->FormatCount.real = true;
                    return false;
                }else{
                    return true;
                }
            }else if(format == "%s\n"){
                if(!this->FormatCount.string){
                    this->FormatCount.string = true;
                    return false;
                }else{
                    return true;
                }
            }
        }

    }  // namespace codegen
}  // namespace otter