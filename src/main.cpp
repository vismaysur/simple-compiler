#include <iostream>
#include <fstream>
#include <sstream>

#include "generation.hpp"

int main(int argc, char* argv[]) {  
    if (argc != 4) {
        std::cerr << "Incorrect usage" << std::endl; 
        std::cerr << "usage: hydro <input.hy> -o <output.asm>" << std::endl;
        return EXIT_FAILURE;
    }
    
    std::stringstream contents_stream;
    std::fstream input(argv[1], std::ios::in);

    if (input.is_open()) {
        contents_stream << input.rdbuf();
        input.close();
    } else {
        std::cerr << "Unable to open input file" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::fstream output(argv[3], std::ios::out);

    Tokenizer tokenizer(std::move(contents_stream.str()));
    std::vector<Token> tokens = tokenizer.tokenize();

    Parser parser(std::move(tokens));
    std::optional<NodeProg> prog = parser.parse_prog();

    if (!prog.has_value()) {
        std::cerr << "Invalid Program" << std::endl;
        exit(EXIT_FAILURE);
    }

    Generator generator(std::move(prog.value()));

    if (output.is_open()) {
        output << generator.gen_prog();
        output.close();
    } else {
        std::cerr << "Unable to open and write to output file" << std::endl;
        exit(EXIT_FAILURE);
    }

    system("nasm -f macho64 ./outputs/actual.asm -o ./outputs/actual.o");
    system("ld -o ./outputs/out ./outputs/actual.o -macos_version_min 10.15 -lSystem -syslibroot /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk");
    system("./outputs/out; echo $?");

    return EXIT_SUCCESS;
}