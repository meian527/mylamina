//
// Created by geguj on 2025/12/28.
//

#pragma once
#include <cstdint>
#include <memory>
#include <vector>
#include <string>

#include "../include/lmx_export.hpp"

// Forward declarations to avoid circular dependencies
namespace lmx {
    class Generator;
    enum class TokenType;
    struct Token;
}

namespace lmx {
static bool node_has_error = false;
enum ASTKind {
    Program,
    Binary, Unary, NumLiteral, StringLiteral, BoolLiteral,
    BlockStmt,
    IfStmt,
    VarDecl,
    VarRef,
    FuncDecl,
    FuncCallExpr,
    Return,
};

struct LMC_API ASTNode {
    ASTKind kind;

    virtual ~ASTNode() = default;
    explicit ASTNode(ASTKind kind) : kind(kind) {}
};

struct TypeNode {
    std::string name;
    
    explicit TypeNode(std::string name) : name(std::move(name)) {}
    ~TypeNode() = default;
    
    TypeNode() = default;
};

struct LMC_API ProgramASTNode final : public ASTNode {
    std::vector<std::shared_ptr<ASTNode>> children;
    
    explicit ProgramASTNode(
        std::vector<std::shared_ptr<ASTNode>> children
    ) : ASTNode(Program),
        children(std::move(children)) {}
};

struct StmtNode : public ASTNode {
    explicit StmtNode(ASTKind kind) : ASTNode(kind) {}
    
    ~StmtNode() override = default;
};

struct ExprNode : public ASTNode {
    explicit ExprNode(ASTKind kind) : ASTNode(kind) {}
    
    ~ExprNode() override = default;
};

struct BlockStmtNode final : public StmtNode {
    std::vector<std::shared_ptr<ASTNode>> children;
    
    explicit BlockStmtNode(std::vector<std::shared_ptr<ASTNode>> children) 
        : StmtNode(ASTKind::BlockStmt), 
          children(std::move(children)) {}
};

struct IfStmtNode : public StmtNode {
    std::shared_ptr<ExprNode> condition;
    std::shared_ptr<BlockStmtNode> thenBlock;
    std::shared_ptr<BlockStmtNode> elseBlock;
    
    explicit IfStmtNode(
        std::shared_ptr<ExprNode> condition,
        std::shared_ptr<BlockStmtNode> thenBlock,
        std::shared_ptr<BlockStmtNode> elseBlock
    ) : StmtNode(ASTKind::IfStmt),
        condition(std::move(condition)),
        thenBlock(std::move(thenBlock)),
        elseBlock(std::move(elseBlock)) {}

};

struct FuncDeclNode final : public ASTNode {
    std::string name;
    std::vector<std::string> args;
    std::shared_ptr<ASTNode> body;
    
    explicit FuncDeclNode(
        std::string name,
        std::vector<std::string> args,
        std::shared_ptr<BlockStmtNode> body
    ) : ASTNode(ASTKind::FuncDecl),
        name(std::move(name)), 
        args(std::move(args)), 
        body(std::move(body)) {}
};

struct ReturnStmtNode final : public StmtNode {
    std::shared_ptr<ASTNode> expr;
    
    explicit ReturnStmtNode(std::shared_ptr<ExprNode> expr) 
        : StmtNode(ASTKind::Return), 
          expr(std::move(expr)) {}
};

struct FuncCallExprNode final : public ExprNode {
    std::string name;
    std::vector<std::shared_ptr<ASTNode>> args;
    
    FuncCallExprNode(
        std::string name,
        std::vector<std::shared_ptr<ASTNode>> args
    ) : ExprNode(ASTKind::FuncCallExpr), 
        name(std::move(name)), 
        args(std::move(args)) {}
};

struct VarDeclNode final : public ASTNode {
    std::string name;
    std::shared_ptr<ASTNode> value;
    bool is_mut;
    
    explicit VarDeclNode(
        std::string name,
        std::shared_ptr<ASTNode> value,
        bool is_mut = true
    ) : ASTNode(VarDecl), 
        name(std::move(name)), 
        value(std::move(value)), 
        is_mut(is_mut) {}
};

struct VarRefNode final : public ExprNode {
    std::string name;
    
    explicit VarRefNode(std::string name) 
        : ExprNode(ASTKind::VarRef), 
          name(std::move(name)) {}
};

struct NumberNode final : public ExprNode {
    std::string num;
    
    explicit NumberNode(std::string num) 
        : ExprNode(ASTKind::NumLiteral), 
          num(std::move(num)) {}
    
    ~NumberNode() override = default;
};

struct BinaryNode final : public ExprNode {
    std::shared_ptr<ASTNode> left;
    std::shared_ptr<ASTNode> right;
    std::string op;
    
    BinaryNode(
        std::shared_ptr<ASTNode> left,
        std::shared_ptr<ASTNode> right,
        std::string op
    ) : ExprNode(ASTKind::Binary), 
        left(std::move(left)), 
        right(std::move(right)), 
        op(std::move(op)) {}
};

struct UnaryNode final : public ExprNode {
    std::string op;
    std::shared_ptr<ASTNode> operand;
    
    explicit UnaryNode(std::string op, std::shared_ptr<ASTNode> operand) 
        : ExprNode(ASTKind::Unary), 
          op(std::move(op)), 
          operand(std::move(operand)) {}
};

} // namespace lmx
