#ifndef _OTTER_CODEGEN_HELPER_HPP_
#define _OTTER_CODEGEN_HELPER_HPP_

namespace otter {
    namespace codegen {
        namespace detail {

            template <typename T>
            decltype(auto) rawPtr(std::shared_ptr<T> ptr) {
                return ptr.get();
            }

            template <typename Type, typename T>
            decltype(auto) sharedIsa(std::shared_ptr<T> ptr) {
                return llvm::isa<Type>(rawPtr(ptr));
            }

            template <typename Type, typename T>
            decltype(auto) sharedCast(std::shared_ptr<T> ptr) {
                return llvm::dyn_cast<Type>(rawPtr(ptr));
            }

            template <typename Type, typename T>
            decltype(auto) shared4Shared(std::shared_ptr<T> ptr) {
                return std::dynamic_pointer_cast<Type>(ptr);
            }

            decltype(auto) constantGet(std::shared_ptr<ast::baseAST> ast,
                                       ast::TypeID type,
                                       auto& context) -> llvm::Value* {
                if (auto rawVal = sharedCast<ast::numberAST>(ast)) {
                    if(rawVal->Type != type){
                        if (rawVal->Type == ast::TypeID::Int) {
                            throw std::string("invalid conversion from 'Int' to 'Double'");
                        }else{
                            throw std::string("invalid conversion from 'Double' to 'Int'");
                        }
                    }
                    if (rawVal->Type == ast::TypeID::Int) {
                        auto valueType = llvm::Type::getInt32Ty(context);
                        return llvm::ConstantInt::getSigned(valueType,
                                                            rawVal->Val);
                    } else {
                        auto valueType = llvm::Type::getDoubleTy(context);
                        return llvm::ConstantFP::get(valueType, rawVal->Val);
                    }
                }
            }

            decltype(auto) type2type(ast::TypeID& type,auto& context) -> llvm::Type*{
                if(type == ast::TypeID::Int){
                    return llvm::Type::getInt32Ty(context);
                }else if(type == ast::TypeID::Double){
                    return llvm::Type::getDoubleTy(context);
                }else if(type == ast::TypeID::String){
                    return llvm::Type::getInt8Ty(context);
                }
            }
            decltype(auto) type2type(llvm::Type*& type,auto& context){
                if(type == llvm::Type::getInt32Ty(context)){
                    return ast::TypeID::Int;
                }else if(type == llvm::Type::getDoubleTy(context)){
                    return ast::TypeID::Double;
                }
            }

            decltype(auto) stdOutType(llvm::Type* Type)
                -> std::string{
                if(Type->getPointerElementType()->getTypeID() == 14){
                    return "%s\n";
                }else if(Type->getPointerElementType()->getTypeID() == 11){
                    return "%d\n";
                }else if(Type->getPointerElementType()->getTypeID() == 3){
                    return "%lf\n";
                }else if(Type->getPointerElementType()->getTypeID() == 12){
                }
            }

        }  // name space detail
    }      // namespace codegen
}  // namespace otter

#endif
