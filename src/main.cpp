#include <iostream>
#include <fstream>
#include <sstream>

enum class TokenType {
    _return,
    int_lit,
    semi,
};

struct Token {
    TokenType type;
    std::optional<std::string> value;
};

std::vector<Token> tokenize(const std::string &str) {
    std::vector<Token> tokens;
    std::string buf;

    for (int i = 0; i < str.length(); i++) {
        char c = str.at(i);
        if (std::isspace(c)) {
            continue;
        } else if (std::isalpha(c)) {
            buf.push_back(c);
            i++;
            while (std::isalnum(str.at(i))) {
                buf.push_back(str.at(i));
                i++;
            }
            if (buf == "return") {
                tokens.push_back({.type = TokenType::_return});
                i--;
                buf.clear();
            } else {
                std::cerr << "u messed up.." << std::endl;
                exit(EXIT_FAILURE);
            }
        } else if (std::isdigit(c)) {
            buf.push_back(c);
            i++;
            while (std::isdigit(str.at(i))) {
                buf.push_back(str.at(i));
                i++;
            }
            tokens.push_back({.type = TokenType::int_lit, .value = buf});
            buf.clear();
            i--;
        } else if (c == ';') {
            tokens.push_back({.type = TokenType::semi});
        }
    }

    return tokens;
};

std::string tokens_to_asm(const std::vector<Token> &tokens) {
    std::stringstream output;
    output << "global _main\nsection .text\n\n_main:\n";

    for (int i = 0; i < tokens.size(); i++) {
        const Token &token = tokens.at(i);
        if (token.type == TokenType::_return) {
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

    std::string contents = contents_stream.str();
    std::vector<Token> tokens = tokenize(contents);
    std::fstream output(argv[3], std::ios::out);

    if (output.is_open()) {
        output << tokens_to_asm(tokens);
        output.close();
    } else {
        std::cerr << "Unable to open output file" << std::endl;
    }

    return EXIT_SUCCESS;
}