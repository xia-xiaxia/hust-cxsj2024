#ifndef LEXER_H
#define LEXER_H

#include "TokenTypes.h"
#include <vector>
#include <string>
#include <stdexcept>

/**
 * 词法错误异常类
 */
class LexicalError : public std::exception {
public:
    std::string message;
    int line;
    int column;
    
    LexicalError(const std::string& msg, int line, int column);
    const char* what() const noexcept override;
    std::string getFullMessage() const;
};

/**
 * 词法分析器类
 * 将源代码文本转换为Token序列
 */
class Lexer {
private:
    std::string text;           // 待分析的文本
    size_t pos;                 // 当前位置
    int line;                   // 当前行号
    int column;                 // 当前列号
    std::vector<Token> tokens;  // 生成的token列表
    std::vector<LexicalError> errors;  // 词法错误列表
    
    // 私有辅助方法
    char currentChar() const;
    char peekChar(int offset = 1) const;
    void advance();
    void skipWhitespace();
    bool skipComment();
    Token readNumber();
    Token readIdentifier();
    Token readString();
    Token createErrorToken(const std::string& message);
    
public:
    // 构造函数
    explicit Lexer(const std::string& text);
    
    // 公共方法
    Token getNextToken();
    std::vector<Token> tokenize();
    
    // 获取错误信息
    const std::vector<LexicalError>& getErrors() const;
    bool hasErrors() const;
    void printErrors() const;
    
    // 重置分析器状态
    void reset();
    void reset(const std::string& newText);
};

#endif // LEXER_H
