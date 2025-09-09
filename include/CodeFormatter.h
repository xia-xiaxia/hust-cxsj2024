#ifndef CODEFORMATTER_H
#define CODEFORMATTER_H

#include <string>
#include <vector>
#include <memory>
#include "TokenTypes.h"
#include "Parser.h"

/**
 * 代码格式化器类
 * 负责将解析后的代码重新格式化输出
 */
class CodeFormatter {
private:
    std::vector<Token> tokens;
    size_t currentToken;
    int indentLevel;
    const int INDENT_SIZE = 4; // 缩进大小（空格数）
    
    // 辅助方法
    std::string getIndent() const;
    std::string formatToken(const Token& token) const;
    bool needsSpaceBefore(const Token& current, const Token& previous) const;
    bool needsSpaceAfter(const Token& current, const Token& next) const;
    bool shouldNewlineBefore(const Token& current, const Token& previous) const;
    bool shouldNewlineAfter(const Token& current, const Token& next) const;
    bool shouldIndentIncrease(const Token& token) const;
    bool shouldIndentDecrease(const Token& token) const;
    
public:
    CodeFormatter();
    
    /**
     * 格式化token流
     * @param tokens 要格式化的token流
     * @return 格式化后的代码字符串
     */
    std::string format(const std::vector<Token>& tokens);
    
    /**
     * 设置缩进大小
     */
    void setIndentSize(int size);
};

#endif // CODEFORMATTER_H
