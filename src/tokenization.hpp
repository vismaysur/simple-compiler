#pragma once

enum class TokenType {
    exit,
    int_lit,
    semi,
    open_paren,
    close_paren,
    ident,
    let,
    eq,
    plus,
    minus,
    multiply,
    divide,
    open_curly,
    close_curly,
};

std::optional<int> bin_prec(const TokenType type) {
    switch(type) {
        case TokenType::plus:
        case TokenType::minus:
            return 0;
        case TokenType::multiply:
        case TokenType::divide:
            return 1;
        default:
            return {};
    }
}



struct Token {
    TokenType type;
    std::optional<std::string> value;
};

class Tokenizer {
public:
    explicit inline Tokenizer(std::string src)
        : m_src(std::move(src)) {

    }

    inline std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        std::string buf;

        while (peek().has_value()) {
            if (std::isalpha(peek().value())) {
                buf.push_back(consume());
                while (peek().has_value() && std::isalnum(peek().value())) {
                    buf.push_back(consume());
                }
                if (buf == "exit") {
                    tokens.push_back({.type = TokenType::exit});
                    buf.clear();
                } else if (buf == "let") {
                    tokens.push_back({.type = TokenType::let});
                    buf.clear();
                } else {
                    tokens.push_back({.type = TokenType::ident, .value = buf});
                    buf.clear();
                }
            } else if (std::isdigit(peek().value())) {
                buf.push_back(consume());
                while (peek().has_value() && std::isdigit(peek().value())) {
                    buf.push_back(consume());
                }
                tokens.push_back({ .type = TokenType::int_lit, .value = buf });
                buf.clear();
            } else if (peek().value() == '(') {
                consume();
                tokens.push_back({ .type = TokenType::open_paren });
            } else if (peek().value() == ')') {
                consume();
                tokens.push_back({ .type = TokenType::close_paren });
            } else if (peek().value() == ';') {
                consume();
                tokens.push_back({.type = TokenType::semi});
            } else if (peek().value() == '=') {
                consume();
                tokens.push_back({.type = TokenType::eq});
            } else if (peek().value() == '+') {
                consume();
                tokens.push_back({.type = TokenType::plus});
            } else if (peek().value() == '-') {
                consume();
                tokens.push_back({.type = TokenType::minus});
            } else if (peek().value() == '*') {
                consume();
                tokens.push_back({.type = TokenType::multiply});
            } else if (peek().value() == '/') {
                consume();
                tokens.push_back({.type = TokenType::divide});
            }  else if (peek().value() == '{') {
                consume();
                tokens.push_back({.type = TokenType::open_curly});
            }  else if (peek().value() == '}') {
                consume();
                tokens.push_back({.type = TokenType::close_curly});
            }  else if (std::isspace(peek().value())) {
                consume();
            } else {
                std::cerr << "Error in tokenization" << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        return tokens;
    }

private:
    [[nodiscard]] inline std::optional<char> peek(int offset = 0) const {
        if (m_index + offset >= m_src.length()) {
            return {};
        } else {
            return m_src.at(m_index + offset);
        }
    }

    inline char consume() {
        return m_src.at(m_index++);
    }

    const std::string m_src;
    size_t m_index = 0;
};