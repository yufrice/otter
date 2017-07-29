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
                    if (type == ast::TypeID::Int) {
                        auto valueType = llvm::Type::getInt32Ty(context);
                        return llvm::ConstantInt::getSigned(valueType,
                                                            rawVal->Val);
                    } else {
                        auto valueType = llvm::Type::getDoubleTy(context);
                        return llvm::ConstantFP::get(valueType, rawVal->Val);
                    }
                }
            }
        }  // name space detail
    }      // namespace codegen
}  // namespace otter

#endif
