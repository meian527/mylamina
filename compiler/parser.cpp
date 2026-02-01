//
// Created by geguj on 2025/12/28.
//

#include "parser.hpp"

#include <stack>

#include "../include/opcode.hpp"

namespace lmx {

void Parser::advance() {
    if (pos < tokens.size()) {
        pos++;
    }
}

Token& Parser::cur() const {
    if (pos >= tokens.size()) {
        return tokens.back();
    }
    return tokens[pos];
}

bool Parser::match(TokenType t) const {
    return cur().type == t;
}

bool Parser::is_eof() const {
    return pos >= tokens.size();
}

void Parser::check_eof() {
    while (!is_eof() && match(TokenType::END_OF_FILE)) {
        advance();
    }
    //if (!is_eof()) {
    //    error("Expected end of file");
    //}
}
void Parser::error(const std::string& msg) {
    has_err = true;
    std::cerr << "Error: " << msg << " at " << cur().line << ":" << cur().col << std::endl;
}
std::shared_ptr<BlockStmtNode> Parser::parse_block() {
    if (!match(TokenType::LBRACE)) error("expected '{'");
    std::vector<std::shared_ptr<ASTNode>> stmts;
    while (!match(TokenType::RBRACE) && !is_eof()) {
        if (const auto stmt = parse()) stmts.push_back(stmt);
    }
    if (!match(TokenType::RBRACE)) error("expected '}'");
    advance();
    return std::make_shared<BlockStmtNode>(stmts);
}
std::shared_ptr<ExprNode> Parser::parse_expr() {
    std::shared_ptr<ExprNode> node = expr();
    if (match(TokenType::EQ) || match(TokenType::LT) || match(TokenType::GT) ||
        match(TokenType::LE) || match(TokenType::GE)) {
        auto op = cur().text;
        advance();
        node = std::make_shared<BinaryNode>(node, parse_expr(), op);
    }
    return node;
}
std::shared_ptr<ASTNode> Parser::parse_if() {
    if (!match(TokenType::LPAREN)) error("expected '('");
    advance();
    std::shared_ptr<ExprNode> condition = parse_expr();
    if (!match(TokenType::RPAREN)) error("expected ')'");

    advance();
    std::shared_ptr<BlockStmtNode> then_block = parse_block();
    std::shared_ptr<BlockStmtNode> else_block = nullptr;
    if (match(TokenType::KW_ELSE)) {
        advance();
        if (match(TokenType::KW_IF)) {
            advance();
            else_block = std::make_shared<BlockStmtNode>(std::vector<std::shared_ptr<ASTNode>>{});
            else_block->children.push_back(parse_if());
        } else {
            else_block = parse_block();
        }
    }
    return std::make_shared<IfStmtNode>(condition, then_block, else_block);
}
std::shared_ptr<ASTNode> Parser::parse() {
    static bool in_func = false;
    std::shared_ptr<ASTNode> node;
    switch (cur().type) {
    case TokenType::KW_LET: {
        advance();
        if (!match(TokenType::IDENTIFIER)) error("expected identifier");
        auto name = cur().text;
        advance();
        if (!match(TokenType::ASSIGN)) error("expected assignment");
        advance();
        node = std::make_shared<VarDeclNode>(name, expr(), false);
        break;
    }
    case TokenType::KW_FUNC: {
        advance();
        in_func = true;
        if (!match(TokenType::IDENTIFIER)) {
            advance();
            error("expected identifier");
            break;
        }
        auto name = cur().text;
        advance();
        if (!match(TokenType::LPAREN)) {
            advance();
            error("expected '('");
            break;
        }
        advance();
        std::vector<std::string> params;
        while (true) {
            if (match(TokenType::IDENTIFIER)) {
                params.push_back(cur().text);
                advance();
            } else if (match(TokenType::RPAREN)) {
                advance();
                break;
            } else if (match(TokenType::COMMA)) {
                advance();
            } else {
                advance();
                error("expected identifier, ',' or ')'");
                break;
            }
        }
        node = std::make_shared<FuncDeclNode>(name, params, parse_block());
        in_func = false;
        break;
    }
    case TokenType::KW_RETURN: {
        advance();
        auto e = expr();
        if (!in_func) {
            error("expected 'return'");
        }
        node = std::make_shared<ReturnStmtNode>(e);
        break;
    }
    case TokenType::KW_IF: {
        advance();
        node = parse_if();
        advance();
        break;
    }
    default: {
        if (match(TokenType::IDENTIFIER) && peek_match(TokenType::ASSIGN)) {
            auto name = cur().text;
            advance();
            advance();
            node = std::make_shared<VarDeclNode>(name, expr());
        } else node = expr();
        break;
    }
    }
    return node;
}
std::shared_ptr<ASTNode> Parser::parse_funcdecl() {
    if (!match(TokenType::IDENTIFIER)) error("expected identifier");
    auto name = cur().text;
    advance();
    if (!match(TokenType::LPAREN)) error("expected '('");
    advance();
    std::vector<std::string> params;
    while (true) {
        if (!match(TokenType::IDENTIFIER)) {
            error("expected identifier");
            return nullptr;
        }
        params.push_back(cur().text);
        advance();
        if (match(TokenType::RPAREN)) break;
        if (!match(TokenType::COMMA)) {
            error("expected ','");
            return nullptr;
        }
    }
    advance();
    return std::make_shared<FuncDeclNode>(name, params, parse_block());
}

std::shared_ptr<ExprNode> Parser::expr() {
    auto node = term();
    while (match(TokenType::OPER_PLUS) || match(TokenType::OPER_MINUS)) {
        auto op = cur().text;
        advance();
        node = std::make_shared<BinaryNode>(node, term(), op);
    }
    return node;
}
std::shared_ptr<ExprNode> Parser::term() {
    auto node = factor();
    while (match(TokenType::OPER_MUL) || match(TokenType::OPER_DIV) || match(TokenType::OPER_MOD) || match(TokenType::OPER_POW)) {
        auto op = cur().text;
        advance();
        node = std::make_shared<BinaryNode>(node, factor(), op);
    }
    return node;
}
std::shared_ptr<ProgramASTNode> Parser::parse_program() {
    std::vector<std::shared_ptr<ASTNode>> stmts;
    while (!match(TokenType::RBRACE) && !is_eof()) {
        if (const auto stmt = parse()) stmts.push_back(stmt);
    }
    return std::make_shared<ProgramASTNode>(stmts);
}

std::shared_ptr<ExprNode> Parser::factor() {
    std::shared_ptr<ExprNode> fact = nullptr;
    if (match(TokenType::NUM_LITERAL)) {
        fact = std::make_shared<NumberNode>(cur().text);
        advance();
    } else if (match(TokenType::LPAREN)) {
        advance();
        fact = expr();
        if (match(TokenType::RPAREN)) {
            advance();
        } else {
            error("Missing closing ')'");
        }
    } else if (match(TokenType::OPER_MINUS) || match(TokenType::OPER_PLUS)) {
        auto op = cur().text;
        advance();
        fact = std::make_shared<UnaryNode>(op, expr());
    } else if (match(TokenType::IDENTIFIER)) {
        auto name = cur().text;
        advance();
        if (!match(TokenType::LPAREN)) fact = make_shared<VarRefNode>(name);
        else {
            advance();
            std::vector<std::shared_ptr<ASTNode>> args;
            while (true) {
                if (match(TokenType::RPAREN)) {
                    advance();
                    break;
                }
                args.push_back(expr());
                if (match(TokenType::COMMA)) advance();
                else if (match(TokenType::RPAREN)) {
                    advance();
                    break;
                }
                else {
                    error("Missing ',' or ')'");
                    advance();
                    break;
                }
            }
            fact = std::make_shared<FuncCallExprNode>(name, args);
        }

    } else {
        //error("unknown token: `" + cur().text + "`");
        advance();
    }
    return fact;
}

bool Parser::peek_match(TokenType type) const {
    return tokens[pos + 1].type == type;
}
} // lmx