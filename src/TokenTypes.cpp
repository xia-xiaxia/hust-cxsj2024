#include "../include/TokenTypes.h"

// Token类实现
Token::Token() : type(TokenType::EOF_TOKEN), value(""), line(0), column(0) {}

Token::Token(TokenType type, const std::string& value, int line, int column)
    : type(type), value(value), line(line), column(column) {}

std::string Token::toString() const {
    return TokenTypeUtils::tokenTypeToString(type) + "(" + value + ") at " 
           + std::to_string(line) + ":" + std::to_string(column);
}

std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << token.toString();
    return os;
}

// TokenTypeUtils类实现
std::unordered_map<std::string, TokenType> TokenTypeUtils::keywords = {
    {"int", TokenType::INT},
    {"float", TokenType::FLOAT_KW},
    {"char", TokenType::CHAR},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"while", TokenType::WHILE},
    {"for", TokenType::FOR},
    {"return", TokenType::RETURN},
    {"void", TokenType::VOID},
    {"break", TokenType::BREAK},
    {"continue", TokenType::CONTINUE},
    {"include", TokenType::INCLUDE},
    {"define", TokenType::DEFINE}
};

const std::unordered_map<std::string, TokenType>& TokenTypeUtils::getKeywords() {
    return keywords;
}

bool TokenTypeUtils::isKeyword(const std::string& word) {
    return keywords.find(word) != keywords.end();
}

TokenType TokenTypeUtils::getKeywordType(const std::string& word) {
    auto it = keywords.find(word);
    if (it != keywords.end()) {
        return it->second;
    }
    return TokenType::IDENTIFIER;
}

std::string TokenTypeUtils::tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::INTEGER: return "INTEGER";
        case TokenType::FLOAT: return "FLOAT";
        case TokenType::STRING: return "STRING";
        case TokenType::INT: return "INT";
        case TokenType::FLOAT_KW: return "FLOAT_KW";
        case TokenType::CHAR: return "CHAR";
        case TokenType::IF: return "IF";
        case TokenType::ELSE: return "ELSE";
        case TokenType::WHILE: return "WHILE";
        case TokenType::FOR: return "FOR";
        case TokenType::RETURN: return "RETURN";
        case TokenType::VOID: return "VOID";
        case TokenType::BREAK: return "BREAK";
        case TokenType::CONTINUE: return "CONTINUE";
        case TokenType::ASSIGN: return "ASSIGN";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::MULTIPLY: return "MULTIPLY";
        case TokenType::DIVIDE: return "DIVIDE";
        case TokenType::MODULO: return "MODULO";
        case TokenType::INCREMENT: return "INCREMENT";
        case TokenType::DECREMENT: return "DECREMENT";
        case TokenType::EQ: return "EQ";
        case TokenType::NE: return "NE";
        case TokenType::LT: return "LT";
        case TokenType::LE: return "LE";
        case TokenType::GT: return "GT";
        case TokenType::GE: return "GE";
        case TokenType::AND: return "AND";
        case TokenType::OR: return "OR";
        case TokenType::NOT: return "NOT";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COMMA: return "COMMA";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::LBRACE: return "LBRACE";
        case TokenType::RBRACE: return "RBRACE";
        case TokenType::LANGLE: return "LANGLE";
        case TokenType::RANGLE: return "RANGLE";
        case TokenType::HASH: return "HASH";
        case TokenType::INCLUDE: return "INCLUDE";
        case TokenType::DEFINE: return "DEFINE";
        case TokenType::EOF_TOKEN: return "EOF";
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::WHITESPACE: return "WHITESPACE";
        case TokenType::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}
