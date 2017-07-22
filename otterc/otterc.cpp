#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <experimental/filesystem>
#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include "otterc.hpp"

int main(int argc, char** argv) {
    using namespace boost::spirit;
    using namespace otter;
    namespace fs = std::experimental::filesystem;
    // namespace po = boost::program_options;
    // po::options_description opt("Options");
    // opt.add_options()("help,h", "help");
    try {
        if (argc == 1) {
            throw std::string("No input files");
        }
        fs::path srcPath = argv[1];
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
        if (succces && first == src.end()) {
            // pre check
            semantics::preCheck pck(result);
            pck.check();

            std::error_code err;
            std::string out = std::string(argv[1]) + ".ll";
            llvm::raw_fd_ostream raw_stream(out, err,
                                            llvm::sys::fs::OpenFlags::F_RW);
            codegen::Generator gen;
            llvm::WriteBitcodeToFile(gen.generatorModule(std::move(result)),
                                     raw_stream);
            raw_stream.close();
        } else {
            std::cout << *first << std::endl;
            /* ToDo
             *  error handring
             *  */
            throw std::string("syntax error");
        }
    } catch (const std::string& e) {
        std::cerr << "error: " << e << std::endl;
    }
    return 0;
}
