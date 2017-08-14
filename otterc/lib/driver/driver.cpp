#include <driver/driver.hpp>


namespace otter {
    namespace driver {

    struct llvmInitialize {
        llvmInitialize() {
           LLVMInitializeX86TargetInfo();
           LLVMInitializeX86Target();
           LLVMInitializeX86TargetMC();
           LLVMInitializeX86AsmPrinter();
           LLVMInitializeX86AsmParser();
           auto& registry = *llvm::PassRegistry::getPassRegistry();
           llvm::initializeCore(registry);
           llvm::initializeCodeGen(registry);
           llvm::initializeLoopStrengthReducePass(registry);
           llvm::initializeLowerIntrinsicsPass(registry);
           llvm::initializeUnreachableBlockElimLegacyPassPass(registry);
        }
    };

    void Driver::BinaryOut(){
        static llvmInitialize const llvmInit;
        llvm::Triple triple(llvm::sys::getDefaultTargetTriple());
        this->Context->Module->setTargetTriple(triple.getTriple());
        std::string targetErr;
        llvm::Target const* target = llvm::TargetRegistry::lookupTarget("", triple, targetErr);
        if(!target){
            throw std::string("");
        }

        llvm::TargetOptions options;
        llvm::Reloc::Model relocModel = llvm::Reloc::Static;
        llvm::CodeModel::Model codeModel = llvm::CodeModel::Default;
        llvm::CodeGenOpt::Level optLevel = llvm::CodeGenOpt::Default;
        auto targetMachine =
                target->createTargetMachine(
                    triple.getTriple(),
                    "",
                    "",
                    options,
                    relocModel,
                    codeModel,
                    optLevel);

        //this->Context->Module->setDataLayout(targetMachine->createDataLayout());

        llvm::legacy::PassManager pm;
        pm.add(llvm::createPromoteMemoryToRegisterPass());
        std::error_code err;
        llvm::raw_fd_ostream raw_stream(this->Context->OutputPath, err,
            llvm::sys::fs::F_None);
        auto fileType = llvm::TargetMachine::CGFT_ObjectFile;
        if(targetMachine->addPassesToEmitFile(pm, raw_stream, fileType)){
            throw std::string("");
        }
        //pm.add(llvm::createPrintModulePass(raw_stream));
        pm.run(*(this->Context->Module.get()));
        raw_stream.close();
    }


    } // namespace driver 
} //namespace otter 