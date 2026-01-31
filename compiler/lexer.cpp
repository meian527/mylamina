//
// Created by geguj on 2025/12/28.
//

#include "lexer.hpp"

#include <unordered_map>
#include <iostream>

namespace lmx {

std::ostream& operator<<(std::ostream& os, const Token& t) {
    os << "Token(";
    switch (t.type) {
    case TokenType::END_OF_FILE: os << "END_OF_FILE"; break;
    case TokenType::IDENTIFIER: os << "IDENTIFIER"; break;
    case TokenType::NUM_LITERAL: os << "INT_LITERAL"; break;
    case TokenType::STRING_LITERAL: os << "STRING_LITERAL"; break;
    case TokenType::COMMA: os << "COMMA"; break;
    case TokenType::TRUE_LITERAL: os << "TRUE_LITERAL"; break;
        case TokenType::FALSE_LITERAL: os << "FALSE_LITERAL"; break;
    case TokenType::OPER_PLUS: os << "OPER_PLUS"; break;
    case TokenType::OPER_MINUS: os << "OPER_MINUS"; break;
    case TokenType::OPER_MUL: os << "OPER_MUL"; break;
    case TokenType::OPER_DIV: os << "OPER_DIV"; break;
    case TokenType::OPER_MOD: os << "OPER_MOD"; break;
    case TokenType::EQ: os << "EQ"; break;
    case TokenType::GE: os << "GE"; break;
    case TokenType::GT: os << "GT"; break;
    case TokenType::LE: os << "LE"; break;
    case TokenType::LT: os << "LT"; break;
    case TokenType::COLON: os << "COLON"; break;
    case TokenType::COL_COLON: os << "COL_COLON"; break;
    case TokenType::OPER_POW: os << "OPER_POW"; break;
    case TokenType::ASSIGN: os << "ASSIGN"; break;
    case TokenType::NOT: os << "NOT"; break;
    case TokenType::NE: os << "NE"; break;
    case TokenType::LPAREN: os << "LPAREN"; break;
    case TokenType::RPAREN: os << "RPAREN"; break;
    case TokenType::LBRACK: os << "LBRACK"; break;
    case TokenType::RBRACK: os << "RBRACK"; break;
    case TokenType::LBRACE: os << "LBRACE"; break;
    case TokenType::RBRACE: os << "RBRACE"; break;

    case TokenType::UNKNOWN: os << "UNKNOWN"; break;
    case TokenType::KW_FUNC: os << "KEYWORD_FUNC"; break;
    case TokenType::KW_RETURN: os << "KEYWORD_RETURN"; break;
    default: os << "UNKNOWN";
    }
    os << ", " << t.text << ", " << t.line << ", " << t.col << ')';
    return os;
}

void Lexer::advance() {
    pos++;
    if (src[pos] == '\n') {
        line++;
        col = 1;
    } else col++;
}

Token Lexer::next() {
    while (isspace(src[pos])) {
        advance();
    }
    if (pos >= src.size()) return {TokenType::END_OF_FILE,"", line, col};

    switch (src[pos]) {
        case '+': {
            advance();
            return {TokenType::OPER_PLUS, "+", line, col};
        }
        case '-': {
            advance();
            return {TokenType::OPER_MINUS, "-", line, col};
        }
        case '*': {
            advance();
            return {TokenType::OPER_MUL, "*", line, col};
        }
        case '/': {
            advance();
            return {TokenType::OPER_DIV, "/", line, col};
        }
        case '%': {
            advance();
            return {TokenType::OPER_MOD, "%", line, col};
        }
        case '=': {
            advance();
            if (src[pos] == '=') {
                advance();
                return {TokenType::EQ, "==", line, col};
            }
            return {TokenType::ASSIGN, "=", line, col};
        }
        case '>': {
            advance();
            if (src[pos] == '=') {
                advance();
                return {TokenType::GE, ">=", line, col};
            }
            return {TokenType::GT, ">", line, col};
        }
        case '<': {
            advance();
            if (src[pos] == '=') {
                advance();
                return {TokenType::LE, "<=", line, col};
            }
            return {TokenType::LT, "<", line, col};
        }
        case ':': {
            advance();
            if (src[pos] == ':') {
                advance();
                return {TokenType::COL_COLON, "::", line, col};
            }
            return {TokenType::COLON, ":", line, col};
        }
        case '^': {
            advance();
            return {TokenType::OPER_POW, "^", line, col};
        }
        case '#': {
            while (src[pos] != '\n')
                advance();
            advance();
        }
        case '"': {
            advance();
            std::string str;
            while (src[pos] != '"') {
                str += src[pos];
                advance();
            }
            advance();

            return {TokenType::STRING_LITERAL, str, line, col - str.size()};
        }
        case '(': {
            advance();
            return {TokenType::LPAREN, "(", line, col};
        }
        case ')': {
            advance();
            return {TokenType::RPAREN, ")", line, col};
        }
        case '{': {
            advance();
            return {TokenType::LBRACE, "{", line, col};
        }
        case '}': {
            advance();
            return {TokenType::RBRACE, "}", line, col};
        }
        case '[': {
            advance();
            return {TokenType::LBRACK, "[", line, col};
        }
        case ']': {
            advance();
            return {TokenType::RBRACK, "]", line, col};
        }
        case ',': {
            advance();
            return {TokenType::COMMA, ", ", line, col};
        }
        case '!': {
            advance();
            if (src[pos] == '=') {
                advance();
                return {TokenType::NE, "!=", line, col};
            }
            return {TokenType::NOT, "!", line, col};
        }
        default: {
            if (isdigit(src[pos])) {
                std::string num;
                while (isdigit(src[pos]) || src[pos] == '_') {
                    if (src[pos] == '_') {
                        advance();
                        continue;
                    }
                    num += src[pos];
                    advance();
                }
                return {TokenType::NUM_LITERAL, num, line, col - num.size()};
            }
            if (isalpha(src[pos])) {
                std::string id;
                while (isalnum(src[pos])) {
                    id += src[pos];
                    advance();
                }
                static const std::unordered_map<std::string, TokenType> keywords = {
                    {"func", TokenType::KW_FUNC},
                    {"return", TokenType::KW_RETURN},
                    {"if", TokenType::KW_IF},
                    {"else", TokenType::KW_ELSE},
                    {"let", TokenType::KW_LET}
                };
                if (const auto it = keywords.find(id); it != keywords.end()) {
                    return {it->second, id, line, col - id.size()};
                }
                return {TokenType::IDENTIFIER, id, line, col - id.size()};
            }
        }
    }
    advance();
    return {TokenType::UNKNOWN, "", line, col};
}

std::vector<Token> Lexer::tokenize(std::string& new_src) {
    src = new_src;
    pos = 0;
    line = 1;
    col = 1;
    std::vector<Token> tokens;
    while (pos < src.size()) {
        tokens.push_back(next());
    }
    // Add EOF token at the end
    tokens.push_back({TokenType::END_OF_FILE, "", line, col});
    return tokens;
}

}
