#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include "TokenTypes.h"
#include "Lexer.h"
#include "Parser.h"
#include <vector>
#include <string>
#include <iostream>

/**
 * 错误类型枚举
 */
enum class ErrorType {
    LEXICAL_ERROR,    // 词法错误
    SYNTAX_ERROR      // 语法错误
};

/**
 * 通用错误信息结构
 */
struct ErrorInfo {
    ErrorType type;
    std::string message;
    int line;
    int column;
    std::string context;  // 错误上下文（出错的代码片段）
    
    ErrorInfo(ErrorType t, const std::string& msg, int l, int c, const std::string& ctx = "")
        : type(t), message(msg), line(l), column(c), context(ctx) {}
    
    std::string toString() const;
};

/**
 * 错误处理器类
 * 统一处理词法错误和语法错误，提供格式化的错误报告
 */
class ErrorHandler {
private:
    std::vector<ErrorInfo> errors;
    std::vector<std::string> sourceLines;  // 源代码行，用于显示上下文
    
    // 私有辅助方法
    std::string getErrorTypeString(ErrorType type) const;
    std::string extractSourceContext(int line, int column) const;
    void showErrorContext(const ErrorInfo& error) const;
    
public:
    // 构造函数
    ErrorHandler() = default;
    explicit ErrorHandler(const std::string& sourceCode);
    
    // 设置源代码（用于提供错误上下文）
    void setSourceCode(const std::string& sourceCode);
    
    // 添加错误
    void addLexicalError(const LexicalError& lexError);
    void addSyntaxError(const SyntaxError& syntaxError);
    void addLexicalErrors(const std::vector<LexicalError>& lexErrors);
    void addSyntaxErrors(const std::vector<SyntaxError>& syntaxErrors);
    
    // 错误查询
    bool hasErrors() const;
    bool hasLexicalErrors() const;
    bool hasSyntaxErrors() const;
    size_t getErrorCount() const;
    size_t getLexicalErrorCount() const;
    size_t getSyntaxErrorCount() const;
    
    // 获取错误信息
    const std::vector<ErrorInfo>& getErrors() const;
    std::vector<ErrorInfo> getLexicalErrors() const;
    std::vector<ErrorInfo> getSyntaxErrors() const;
    
    // 错误报告
    void printErrors() const;
    void printSummary() const;
    void printDetailedReport() const;
    
    // 生成报告
    std::string generateErrorReport() const;
    std::string generateSummary() const;
    
    // 清除错误
    void clearErrors();
    
    // 静态辅助方法
    static std::string formatErrorMessage(const std::string& message, int line, int column);
};

#endif // ERRORHANDLER_H
