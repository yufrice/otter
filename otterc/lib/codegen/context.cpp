#include <codegen/context.hpp>

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

        void Context::setType(llvm::Type* type){
            this->argsTypes.emplace_back(type);
        }

        llvm::Type* Context::getType(){
            if(this->argsTypes.empty()){
                return nullptr;
            }
            auto type = this->argsTypes.at(0);
            this->argsTypes.pop_back();
            return type;
        }

        bool Context::clearType(){
            if(this->argsTypes.empty()){
                return true;
            }else{
                this->argsTypes.clear();
                return false;
            }
        }

    }  // namespace codegen
}  // namespace otter