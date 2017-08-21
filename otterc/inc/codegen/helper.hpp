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

            decltype(auto) constantGet(ast::TypeID &type, std::shared_ptr<ast::baseAST> ast,
                                       auto& context) -> llvm::Value* {
                if (auto rawVal = sharedCast<ast::numberAST>(ast)) {
                    if(type != rawVal->Type){
                        throw std::string("invaild conversion");
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

            decltype(auto) stdOutType = [](llvm::Type* Type,std::string& format){
                    if(Type->getTypeID() == 14){
                        format = "%s\n";
                    }else if(Type->getTypeID() == 11){
                        format = "%d\n";
                    }else if(Type->getTypeID() == 3){
                        format = "%lf\n";
                    }
            };

            decltype(auto) formatNameMangling = [](const std::string& name){
                if(name == "%s\n"){
                    return "stringFormat";
                }else if(name == "%d\n"){
                    return "digitFormat";
                }else if(name == "%lf\n"){
                    return "realFormat";
                }
            };

            namespace{
                decltype(auto) equalPair = [](const auto &pair,const auto& str1, const auto& str2){
                                if(pair.first == str1 && pair.second == str2){
                                    return true;
                                }else{
                                    false;
                                }
                };
            } // name space any

            decltype(auto) op2op = [](const std::string& op,const ast::TypeID& type){
                auto pair = std::make_pair(op,type);
                if(equalPair(pair,"+",ast::TypeID::Int)){
                    return llvm::Instruction::Add; 
                }else if(equalPair(pair,"+",ast::TypeID::Double)){
                    return llvm::Instruction::FAdd; 
                }else if(equalPair(pair,"-",ast::TypeID::Int)){
                    return llvm::Instruction::Sub; 
                }else if(equalPair(pair,"-",ast::TypeID::Double)){
                    return llvm::Instruction::FSub; 
                }else if(equalPair(pair,"*",ast::TypeID::Int)){
                    return llvm::Instruction::Mul; 
                }else if(equalPair(pair,"*",ast::TypeID::Double)){
                    return llvm::Instruction::FMul; 
                }else if(equalPair(pair,"/",ast::TypeID::Int)){
                    return llvm::Instruction::SDiv; 
                }else if(equalPair(pair,"/",ast::TypeID::Double)){
                    return llvm::Instruction::FDiv; 
                }
            };

        }  // name space detail
    }      // namespace codegen
}  // namespace otter

#endif
