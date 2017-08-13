#include <llvm/LinkAllPasses.h>
#include <llvm/IR/LegacyPassManager.h>

#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/CommandLine.h>
#include <experimental/filesystem>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <otterc.hpp>


int main(int argc, char** argv) {
    using namespace boost::spirit;
    using namespace otter;
    namespace fs = std::experimental::filesystem;

    static llvm::cl::opt<std::string> InputFilename(llvm::cl::Positional, llvm::cl::desc("<input file>"));
    static llvm::cl::opt<std::string> OutputFilename("o", llvm::cl::desc("Place the output into <file>."), llvm::cl::value_desc("file"), llvm::cl::init("a.ll"));
    llvm::cl::ParseCommandLineOptions(argc, argv);
    try {
        std::ostringstream Input(InputFilename.c_str());
        if (!Input.good()) {
            throw std::string("No input files");
        }
        fs::path srcPath(Input.str());
        if (!fs::exists(srcPath)) {
            throw(srcPath.string() + ": No such file");
        }
        std::ifstream ifs(srcPath);
        if (ifs.fail()) {
            throw std::string("Failed to open");
        }

        std::string src;
        {
            std::string tmp;
            while (getline(ifs, tmp)) {
                src += tmp;
                src += "\n";
            }
        }

        auto result  = std::make_shared<ast::moduleAST>();
        auto first   = src.begin();
        auto succces = x3::phrase_parse(first, src.end(), parser::module,
                                        parser::skkiper, result);
        if (result && succces && first == src.end()) {
            // pre check
            semantics::preCheck pck(result);
            pck.check();

            llvm::legacy::PassManager pm;
            pm.add(llvm::createPromoteMemoryToRegisterPass());
            std::error_code err;
            std::string out(OutputFilename.c_str());
            llvm::raw_fd_ostream raw_stream(out, err,
                                            llvm::sys::fs::OpenFlags::F_RW);
            auto gen = std::unique_ptr<codegen::Generator>(new codegen::Generator);
            std::unique_ptr<llvm::Module> Module(gen->generatorModule(result));
            pm.add(llvm::createPrintModulePass(raw_stream));
            pm.run(*(Module.get()));
            raw_stream.close();

        } else {
            /* ToDo
             *  error handring
             *  */
            std::string error = "\x1b[31m";
            for (; *first != '\n'; first++) {
                error += *first;
            }
            error += "\x1b[0m";
            throw std::string("syntax error :\n\t" + error);
        }
    } catch (const std::string& e) {
        std::cerr << "\x1b[31m";
        std::cerr << "error: ";
        std::cerr << "\x1b[0m";
        std::cerr << e << std::endl;
    }

    return 0;
}
