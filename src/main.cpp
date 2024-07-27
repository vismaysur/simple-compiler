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
    std::optional<NodeExit> tree = parser.parse();

    if (!tree.has_value()) {
        std::cerr << "No exit statement found" << std::endl;
        exit(EXIT_FAILURE);
    }

    Generator generator(tree.value());

    if (output.is_open()) {
        output << generator.generate();
        output.close();
    } else {
        std::cerr << "Unable to open output file" << std::endl;
        exit(EXIT_FAILURE);
    }

    system("nasm -f macho64 ./outputs/actual.asm -o ./outputs/actual.o");
    // system("ld -o ./outputs/out ./outputs/actual.o -macos_version_min 10.15 -lSystem");
    // system("./outputs/out");

    return EXIT_SUCCESS;
}