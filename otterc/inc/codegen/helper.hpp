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

            template <typename... Str>
            decltype(auto) typeError(std::string errType, const Str... str)
                -> std::string {
                std::vector<std::string> strs = {str...};
                if (errType == "invConv" && strs.size() == 3) {
                    return std::string("invalid conversion from '") +
                           strs.at(0) + std::string("' to '") + strs.at(1) +
                           std::string("' : ") + strs.at(2);
                } else if (errType == "noValue" && strs.size() == 1) {
                    return strs.at(0) + std::string(" has no Value");
                }
            }

            decltype(auto) constantGet(ast::TypeID& type, auto& context)
                -> llvm::Constant* {
                if (type == ast::TypeID::Int) {
                    auto valueType = llvm::Type::getInt32Ty(context);
                    return llvm::ConstantInt::getSigned(valueType, 0);
                } else if (type == ast::TypeID::Double) {
                    auto valueType = llvm::Type::getDoubleTy(context);
                    return llvm::ConstantFP::get(valueType, 0);
                }
            }

            decltype(auto) constantGet(std::shared_ptr<ast::baseAST> ast,
                                       auto& context) -> llvm::Value* {
                if (auto rawVal = sharedCast<ast::numberAST>(ast)) {
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

            decltype(auto) type2type(ast::TypeID& type, auto& context)
                -> llvm::Type* {
                if (type == ast::TypeID::Int) {
                    return llvm::Type::getInt32Ty(context);
                } else if (type == ast::TypeID::Double) {
                    return llvm::Type::getDoubleTy(context);
                } else if (type == ast::TypeID::Bool) {
                    return llvm::Type::getInt1Ty(context);
                } else if (type == ast::TypeID::String) {
                    return llvm::Type::getInt8Ty(context);
                }
            }
            decltype(auto) type2type(llvm::Type* type, auto& context) {
                if (type->isIntegerTy(32)) {
                    return ast::TypeID::Int;
                } else if (type->isIntegerTy(1)) {
                    return ast::TypeID::Bool;
                } else if (type->isDoubleTy()) {
                    return ast::TypeID::Double;
                } else if (type->isArrayTy() || type->isIntegerTy(8)) {
                    return ast::TypeID::String;
                } else if (type->isFunctionTy()) {
                    return ast::TypeID::Nil;
                }
            }

            decltype(auto) stdOutType = [](llvm::Type* Type,
                                           std::string& format) {
                if (Type->isIntegerTy(32)) {
                    format = "%d\n";
                } else if (Type->isDoubleTy()) {
                    format = "%lf\n";
                }else {
                    format = "%s\n";
                }
            };

            decltype(auto) formatNameMangling = [](const std::string& name) {
                if (name == "%s\n") {
                    return "stringFormat";
                } else if (name == "%d\n") {
                    return "digitFormat";
                } else if (name == "%lf\n") {
                    return "realFormat";
                }
            };

            decltype(auto) i12Bool  = [](const llvm::Value* val) -> std::string {
                std::cout << val->getPointerAlignment() << std::endl;
                if(auto constant = llvm::dyn_cast<llvm::ConstantInt>(val)){
                    if(constant->isNegativeZeroValue()){
                        return "False";
                    }else{
                        return "True";
                    }
                }
                throw std::string("helper:121");
            };

            decltype(auto) pointerType = [](llvm::Type* type) -> llvm::Type* {
                if (type->isPointerTy()) {
                    if (type->getPointerElementType()->getTypeID() == 12) {
                        auto fType = llvm::dyn_cast<llvm::FunctionType>(
                            type->getPointerElementType());
                        if (fType->getReturnType()->isPointerTy()) {
                            return fType->getReturnType()
                                ->getPointerElementType();
                        } else {
                            return fType->getReturnType();
                        }
                    }
                    return type->getPointerElementType();
                } else {
                    return type;
                }
            };

            namespace {
                decltype(auto) equalPair =
                    [](const auto& pair, const auto& str1, const auto& str2) {
                        if (pair.first == str1 && pair.second == str2) {
                            return true;
                        } else {
                            false;
                        }
                    };
            }  // name space any

            decltype(auto) op2op = [](const std::string& op,
                                      const ast::TypeID& type) {
                auto pair = std::make_pair(op, type);
                if (equalPair(pair, "+", ast::TypeID::Int)) {
                    return llvm::Instruction::Add;
                } else if (equalPair(pair, "+", ast::TypeID::Double)) {
                    return llvm::Instruction::FAdd;
                } else if (equalPair(pair, "-", ast::TypeID::Int)) {
                    return llvm::Instruction::Sub;
                } else if (equalPair(pair, "-", ast::TypeID::Double)) {
                    return llvm::Instruction::FSub;
                } else if (equalPair(pair, "*", ast::TypeID::Int)) {
                    return llvm::Instruction::Mul;
                } else if (equalPair(pair, "*", ast::TypeID::Double)) {
                    return llvm::Instruction::FMul;
                } else if (equalPair(pair, "/", ast::TypeID::Int)) {
                    return llvm::Instruction::SDiv;
                } else if (equalPair(pair, "/", ast::TypeID::Double)) {
                    return llvm::Instruction::FDiv;
                }
            };

            decltype(auto) op2lop = [](const std::string& op,
                                       const ast::TypeID& type) {
                auto pair = std::make_pair(op, type);
                if (equalPair(pair, "=", ast::TypeID::Int)) {
                    return llvm::CmpInst::ICMP_EQ;
                } else if (equalPair(pair, "=", ast::TypeID::Double)) {
                    return llvm::CmpInst::FCMP_OEQ;
                } else if (equalPair(pair, "<>", ast::TypeID::Int)) {
                    return llvm::CmpInst::ICMP_NE;
                } else if (equalPair(pair, "<>", ast::TypeID::Double)) {
                    return llvm::CmpInst::FCMP_ONE;
                } else if (equalPair(pair, ">", ast::TypeID::Int)) {
                    return llvm::CmpInst::ICMP_SGT;
                } else if (equalPair(pair, ">", ast::TypeID::Double)) {
                    return llvm::CmpInst::FCMP_OGT;
                } else if (equalPair(pair, "<", ast::TypeID::Int)) {
                    return llvm::CmpInst::ICMP_SLT;
                } else if (equalPair(pair, "<", ast::TypeID::Double)) {
                    return llvm::CmpInst::FCMP_OLT;
                } else if (equalPair(pair, ">=", ast::TypeID::Int)) {
                    return llvm::CmpInst::ICMP_SGE;
                } else if (equalPair(pair, ">=", ast::TypeID::Double)) {
                    return llvm::CmpInst::FCMP_OGE;
                } else if (equalPair(pair, "<=", ast::TypeID::Int)) {
                    return llvm::CmpInst::ICMP_SLE;
                } else if (equalPair(pair, "<=", ast::TypeID::Double)) {
                    return llvm::CmpInst::FCMP_OLE;
                }
            };

            // std::function<ast::TypeID(std::shared_ptr<baseAST>)>
            // getExprType
            //= [&getExprType](std::shared_ptr<baseAST> expr){
            //};
        }  // name space detail
    }      // namespace codegen
}  // namespace otter

#endif
