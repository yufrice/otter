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

           LLVMInitializeAArch64TargetInfo();
           LLVMInitializeAArch64Target();
           LLVMInitializeAArch64TargetMC();
           LLVMInitializeAArch64AsmPrinter();
           LLVMInitializeAArch64AsmParser();
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
        //llvm::Triple triple(llvm::sys::getDefaultTargetTriple());
        //std::cout << triple.getTriple() << std::endl;
        auto triple = llvm::sys::getDefaultTargetTriple();
        std::string targetErr;
        llvm::Target const* target = llvm::TargetRegistry::lookupTarget(triple, targetErr);
        if(!target){
            throw targetErr;
        }

        llvm::TargetOptions options;
        auto relocModel = llvm::Optional<llvm::Reloc::Model>();
        auto targetMachine =
                target->createTargetMachine(
                    triple,
                    "generic",
                    "",
                    options,
                    relocModel);

        this->Context->Module->setTargetTriple(triple);
        this->Context->Module->setDataLayout(targetMachine->createDataLayout());

        llvm::legacy::PassManager pm;
        //pm.add(llvm::createPromoteMemoryToRegisterPass());
        std::error_code err;
        llvm::raw_fd_ostream raw_stream("out.obj", err,
            llvm::sys::fs::F_None);
        auto fileType = llvm::TargetMachine::CGFT_ObjectFile;
        if(targetMachine->addPassesToEmitFile(pm, raw_stream, fileType)){
            throw std::string("fail gen tartget machine");
        }
        //pm.add(llvm::createPrintModulePass(raw_stream));
        pm.run(*(this->Context->Module));
        this->Context->Module->dump();
        raw_stream.flush();
        std::string const command = "g++ -fPIC -o " + this->Context->OutputPath + " out.obj";
        std::system(command.c_str());
        //raw_stream.close();
    }


    } // namespace driver 
} //namespace otter 