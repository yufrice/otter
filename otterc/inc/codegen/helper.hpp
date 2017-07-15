#ifndef _OTTER_CODEGEN_HELPER_HPP_
#define _OTTER_CODEGEN_HELPER_HPP_


namespace otter {
    namespace codegen {
        namespace detail {
            template<typename T>
            decltype(auto) rawPtr(std::shared_ptr<T> ptr){
                return ptr.get();
            }

            template<typename Type,typename T>
                decltype(auto) sharedIsa(std::shared_ptr<T> ptr){
                    return llvm::isa<Type>(rawPtr(ptr));
                }

            template<typename Type,typename T>
                decltype(auto) sharedCast(std::shared_ptr<T> ptr){
                    return llvm::dyn_cast<Type>(rawPtr(ptr));
                }
        } // name space detail
    } // namespace codegen
} //namespace otter

#endif
