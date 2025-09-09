#ifndef TOKENTYPES_H
#define TOKENTYPES_H

#include <string>
#include <unordered_map>
#include <iostream>

/**
 * Token类型枚举
 * 定义词法分析器识别的所有token类型
 */
enum class TokenType {
    // 字面量
    IDENTIFIER,     // 标识符
    INTEGER,        // 整数
    FLOAT,          // 浮点数
    STRING,         // 字符串
    
    // 关键字
    INT,            // int
    FLOAT_KW,       // float
    CHAR,           // char
    IF,             // if
    ELSE,           // else
    WHILE,          // while
    FOR,            // for
    RETURN,         // return
    VOID,           // void
    BREAK,          // break
    CONTINUE,       // continue
    
    // 操作符
    ASSIGN,         // =
    PLUS,           // +
    MINUS,          // -
    MULTIPLY,       // *
    DIVIDE,         // /
    MODULO,         // %
    INCREMENT,      // ++
    DECREMENT,      // --
    
    // 比较操作符
    EQ,             // ==
    NE,             // !=
    LT,             // <
    LE,             // <=
    GT,             // >
    GE,             // >=
    
    // 逻辑操作符
    AND,            // &&
    OR,             // ||
    NOT,            // !
    
    // 分隔符
    SEMICOLON,      // ;
    COMMA,          // ,
    LPAREN,         // (
    RPAREN,         // )
    LBRACE,         // {
    RBRACE,         // }
    LANGLE,         // <
    RANGLE,         // >
    
    // 预处理
    HASH,           // #
    INCLUDE,        // include
    DEFINE,         // define
    
    // 特殊
    EOF_TOKEN,      // 文件结束
    NEWLINE,        // 换行
    WHITESPACE,     // 空白字符
    
    // 错误
    ERROR           // 无法识别的字符
};

/**
 * Token类，表示词法分析的基本单元
 */
class Token {
public:
    TokenType type;         // Token类型
    std::string value;      // Token值
    int line;              // 行号
    int column;            // 列号
    
    // 构造函数
    Token();
    Token(TokenType type, const std::string& value, int line, int column);
    
    // 输出函数
    std::string toString() const;
    friend std::ostream& operator<<(std::ostream& os, const Token& token);
};

/**
 * 工具类，提供Token类型相关的辅助函数
 */
class TokenTypeUtils {
public:
    // 获取关键字映射表
    static const std::unordered_map<std::string, TokenType>& getKeywords();
    
    // 检查是否为关键字
    static bool isKeyword(const std::string& word);
    
    // 获取关键字对应的TokenType
    static TokenType getKeywordType(const std::string& word);
    
    // 获取TokenType的字符串表示
    static std::string tokenTypeToString(TokenType type);
    
private:
    // 关键字映射表
    static std::unordered_map<std::string, TokenType> keywords;
};

#endif // TOKENTYPES_H
