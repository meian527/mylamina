//
// Common definitions for compiler module
//

#pragma once
#include <string>
#include <vector>

namespace lmx {
    // Forward declarations to avoid circular dependencies
    class Generator;
    
    // TokenType enum definition
    LMC_API enum TokenType {
        END_OF_FILE,
        OPER_PLUS, OPER_MINUS, OPER_MUL, OPER_DIV, OPER_MOD, OPER_POW,

        ASSIGN, COLON, COL_COLON, COMMA, NOT,

        LPAREN, RPAREN, LBRACK, RBRACK, LBRACE, RBRACE,

        EQ, NE, LT, GT, LE, GE,

        NUM_LITERAL, STRING_LITERAL, TRUE_LITERAL, FALSE_LITERAL, IDENTIFIER,

        KW_FUNC, KW_RETURN,
        UNKNOWN, KW_IF, KW_ELSE, KW_LET
    };
    
    // Token struct definition
    struct LMC_API Token {
        TokenType type;
        std::string text;
        size_t line, col;

        LMC_API friend std::ostream& operator<<(std::ostream& os, const Token& t);
    };
}
