#include <iostream>
#include <fstream>
#include <sstream>
#include "tokenization.hpp"

std::string tokens_to_asm(const std::vector<Token> &tokens) {
    std::stringstream output;
    output << "global _main\nsection .text\n\n_main:\n";

    for (int i = 0; i < tokens.size(); i++) {
        const Token &token = tokens.at(i);
        if (token.type == TokenType::exit) {
            if (i+1 < tokens.size() && tokens.at(i+1).type == TokenType::int_lit) {
                if (i+2 < tokens.size() && tokens.at(i+2).type == TokenType::semi) {
                    output << "    mov rax, 0x2000001\n";
                    output << "    mov rdi, " << tokens.at(i+1).value.value() << "\n";
                    output << "    syscall";
                }
            }
        }
    }

    return output.str();
};

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
    }

    Tokenizer tokenizer(std::move(contents_stream.str()));
    std::vector<Token> tokens = tokenizer.tokenize();
    std::fstream output(argv[3], std::ios::out);

    if (output.is_open()) {
        output << tokens_to_asm(tokens);
        output.close();
    } else {
        std::cerr << "Unable to open output file" << std::endl;
    }

    system("nasm -f macho64 ./outputs/actual.asm -o ./outputs/actual.o");
    // system("ld -o ./outputs/out ./outputs/actual.o -macos_version_min 10.15 -lSystem");
    // system("./outputs/out");

    return EXIT_SUCCESS;
}