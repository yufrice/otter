#include <experimental/filesystem>
#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>
#include "parser.hpp"

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
        fs::path p = argv[1];
        if (!fs::exists(p)) {
            throw(p.string() + ": No such file");
        }
        std::ifstream ifs(p);
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
        }  // std::string tmp

        ast::moduleAST result;
        auto first = src.begin();
        auto succces = x3::phrase_parse(first, src.end(), parser::module,
                                        x3::standard_wide::space, result);
        if (succces && first == src.end()) {
            std::cout << "ok" << std::endl;
        } else {
            std::cerr << *first << std::endl;
            throw std::string("syntax error");
        }
    } catch (const std::string& e) {
        std::cerr << "error: " << e << std::endl;
    }
    return 0;
}
