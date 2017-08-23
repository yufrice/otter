#include <iostream>
#include <experimental/filesystem>
#include <llvm/Support/CommandLine.h>
#include <fstream>
#include <memory>
#include <vector>
#include <otterc.hpp>

namespace {
    static llvm::cl::OptionCategory CompilerCategory("Complier Options");
    static llvm::cl::opt<std::string> InputFilename(llvm::cl::Positional,
                                                    llvm::cl::desc("<input>"));
    static llvm::cl::opt<std::string> OutputFilename(
        "o",
        llvm::cl::desc("Place the output into <file>."),
        llvm::cl::value_desc("file"),
        llvm::cl::init("a.out"),
        llvm::cl::cat(CompilerCategory));
    static llvm::cl::opt<bool> DumpOpt("dump",
                                       llvm::cl::desc("Display bitcode."),
                                       llvm::cl::init(false),
                                       llvm::cl::cat(CompilerCategory));
    static llvm::cl::opt<bool> DebugOpt("debug",
                                       llvm::cl::desc("Display debug log."),
                                       llvm::cl::init(false),
                                       llvm::cl::cat(CompilerCategory));
}

int main(int argc, char** argv) {
    using namespace boost::spirit;
    using namespace otter;
    namespace fs = std::experimental::filesystem;

    llvm::cl::HideUnrelatedOptions(CompilerCategory);
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

            auto codeGen =
                std::unique_ptr<codegen::Generator>(new codegen::Generator);
            auto const& context =
                context::Context(std::move(codeGen->generatorModule(result)),
                                 OutputFilename.c_str(), DumpOpt);
            auto driver = driver::Driver(context);
            if(DebugOpt){
                context.Module->dump();
            }else{
                driver.BinaryOut();
            }

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
