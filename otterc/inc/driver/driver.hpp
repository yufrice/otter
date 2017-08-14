#ifndef _OTTER_DRIVER_DRIVER_HPP_
#define _OTTER_DRIVER_DRIVER_HPP_

#include <iostream>
#include <memory>
#include <context/context.hpp>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/LinkAllPasses.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>

namespace otter {
    namespace driver {
        class Driver {
            public:
                Driver(context::Context context):
                    Context(std::make_unique<context::Context>(context)){};
                ~Driver(){};
                void BinaryOut();

            private:
                std::unique_ptr<context::Context> Context;
        };
    } // namespace otter
} //namespace codegen

#endif