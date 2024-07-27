#pragma once

enum class TokenType {
    exit,
    int_lit,
    semi,
};

struct Token {
    TokenType type;
    std::optional<std::string> value;
};

class Tokenizer {
public:
    inline Tokenizer(std::string src)
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
                    continue;
                } else {
                    std::cerr << "you messed up!; here1" << std::endl;
                    exit(EXIT_FAILURE);
                }
            } else if (std::isdigit(peek().value())) {
                buf.push_back(consume());
                while (peek().has_value() && std::isdigit(peek().value())) {
                    buf.push_back(consume());
                }
                tokens.push_back({ .type = TokenType::int_lit, .value = buf });
                buf.clear();
                continue;
            } else if (peek().value() == ';') {
                consume();
                tokens.push_back({.type = TokenType::semi});
                continue;
            } else if (std::isspace(peek().value())) {
                consume();
                continue;
            } else {
                std::cerr << "you messed up!; here2" << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        return tokens;
    }
private:
    [[nodiscard]] inline std::optional<char> peek(int ahead = 1) const {
        if (m_index + ahead - 1 >= m_src.length()) {
            return {};
        } else {
            return m_src.at(m_index + ahead - 1);
        }
    }

    inline char consume() {
        return m_src.at(m_index++);
    }

    const std::string m_src;
    size_t m_index = 0;
};