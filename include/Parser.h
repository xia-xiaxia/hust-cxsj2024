#ifndef PARSER_H
#define PARSER_H

#include "TokenTypes.h"
#include "Lexer.h"
#include <vector>
#include <memory>
#include <stdexcept>

// 前向声明
class ASTNode;

/**
 * 语法错误异常类
 */
class SyntaxError : public std::exception {
public:
    std::string message;
    int line;
    int column;
    
    SyntaxError(const std::string& msg, int line, int column);
    const char* what() const noexcept override;
    std::string getFullMessage() const;
};

/**
 * 抽象语法树节点基类
 */
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual std::string toString() const = 0;
    virtual void print(int indent = 0) const;
    virtual void printChinese(int indent = 0) const;
};

/**
 * 程序节点（根节点）
 */
class ProgramNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;
    
    std::string toString() const override;
    void printChinese(int indent = 0) const override;
    void addStatement(std::unique_ptr<ASTNode> stmt);
};

/**
 * 变量声明节点
 */
class VarDeclarationNode : public ASTNode {
public:
    std::string type;        // 变量类型
    std::string identifier;  // 变量名
    std::unique_ptr<ASTNode> initializer;  // 初始化表达式
    
    VarDeclarationNode(const std::string& type, const std::string& id);
    std::string toString() const override;
    void printChinese(int indent = 0) const override;
};

/**
 * 赋值语句节点
 */
class AssignmentNode : public ASTNode {
public:
    std::string identifier;
    std::unique_ptr<ASTNode> expression;
    
    AssignmentNode(const std::string& id);
    std::string toString() const override;
    void printChinese(int indent = 0) const override;
};

/**
 * 二元表达式节点
 */
class BinaryExpressionNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> left;
    std::string operator_;
    std::unique_ptr<ASTNode> right;
    
    BinaryExpressionNode(const std::string& op);
    std::string toString() const override;
    void printChinese(int indent = 0) const override;
};

/**
 * 一元表达式节点
 */
class UnaryExpressionNode : public ASTNode {
public:
    std::string operator_;
    std::unique_ptr<ASTNode> operand;
    
    UnaryExpressionNode(const std::string& op);
    std::string toString() const override;
    void printChinese(int indent = 0) const override;
};

/**
 * 字面量节点
 */
class LiteralNode : public ASTNode {
public:
    std::string value;
    TokenType type;
    
    LiteralNode(const std::string& val, TokenType t);
    std::string toString() const override;
    void printChinese(int indent = 0) const override;
};

/**
 * 标识符节点
 */
class IdentifierNode : public ASTNode {
public:
    std::string name;
    
    IdentifierNode(const std::string& n);
    std::string toString() const override;
    void printChinese(int indent = 0) const override;
};

/**
 * if语句节点
 */
class IfStatementNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> thenStatement;
    std::unique_ptr<ASTNode> elseStatement;
    
    std::string toString() const override;
    void printChinese(int indent = 0) const override;
};

/**
 * while语句节点
 */
class WhileStatementNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> body;
    
    std::string toString() const override;
    void printChinese(int indent = 0) const override;
};

/**
 * 复合语句节点（代码块）
 */
class CompoundStatementNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;
    
    std::string toString() const override;
    void printChinese(int indent = 0) const override;
    void addStatement(std::unique_ptr<ASTNode> stmt);
};

/**
 * return语句节点
 */
class ReturnStatementNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> expression;
    
    std::string toString() const override;
    void printChinese(int indent = 0) const override;
};

/**
 * 预处理指令节点
 */
class PreprocessorDirectiveNode : public ASTNode {
public:
    std::string directive; // include, define等
    std::string content;   // 指令内容
    
    PreprocessorDirectiveNode(const std::string& dir, const std::string& cont);
    std::string toString() const override;
    void printChinese(int indent = 0) const override;
};

/**
 * 函数声明节点
 */
class FunctionDeclarationNode : public ASTNode {
public:
    std::string returnType;
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> parameters;
    
    FunctionDeclarationNode(const std::string& retType, const std::string& funcName);
    std::string toString() const override;
    void printChinese(int indent = 0) const override;
};

/**
 * 函数定义节点
 */
class FunctionDefinitionNode : public ASTNode {
public:
    std::string returnType;
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> parameters;
    std::unique_ptr<ASTNode> body;
    
    FunctionDefinitionNode(const std::string& retType, const std::string& funcName);
    std::string toString() const override;
    void printChinese(int indent = 0) const override;
};

/**
 * 表达式语句节点
 */
class ExpressionStatementNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> expression;
    
    ExpressionStatementNode(std::unique_ptr<ASTNode> expr);
    std::string toString() const override;
    void printChinese(int indent = 0) const override;
};

/**
 * 函数调用节点
 */
class FunctionCallNode : public ASTNode {
public:
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> arguments;
    
    FunctionCallNode(const std::string& funcName);
    std::string toString() const override;
    void printChinese(int indent = 0) const override;
};

/**
 * For语句节点
 */
class ForStatementNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> initialization;
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> update;
    std::unique_ptr<ASTNode> body;
    
    std::string toString() const override;
    void printChinese(int indent = 0) const override;
};

/**
 * Break语句节点
 */
class BreakStatementNode : public ASTNode {
public:
    std::string toString() const override;
    void printChinese(int indent = 0) const override;
};

/**
 * Continue语句节点
 */
class ContinueStatementNode : public ASTNode {
public:
    std::string toString() const override;
    void printChinese(int indent = 0) const override;
};

/**
 * 语法分析器类
 * 使用递归下降分析方法构建抽象语法树
 */
class Parser {
private:
    std::vector<Token> tokens;
    size_t currentToken;
    std::vector<SyntaxError> errors;
    
    // 当前token相关方法
    const Token& getCurrentToken() const;
    const Token& peekToken(int offset = 1) const;
    bool isAtEnd() const;
    void advance();
    bool match(TokenType type);
    bool check(TokenType type) const;
    Token consume(TokenType type, const std::string& errorMessage);
    
    // 错误处理
    void recordError(const std::string& message);
    void synchronize();
    
    // 语法分析方法（递归下降）
    std::unique_ptr<ProgramNode> parseProgram();
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseVarDeclaration();
    std::unique_ptr<ASTNode> parseAssignment();
    std::unique_ptr<ASTNode> parseIfStatement();
    std::unique_ptr<ASTNode> parseWhileStatement();
    std::unique_ptr<ASTNode> parseForStatement();
    std::unique_ptr<ASTNode> parseCompoundStatement();
    std::unique_ptr<ASTNode> parseReturnStatement();
    std::unique_ptr<ASTNode> parseExpressionStatement();
    
    // 表达式分析
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseLogicalOr();
    std::unique_ptr<ASTNode> parseLogicalAnd();
    std::unique_ptr<ASTNode> parseEquality();
    std::unique_ptr<ASTNode> parseComparison();
    std::unique_ptr<ASTNode> parseAddition();
    std::unique_ptr<ASTNode> parseMultiplication();
    std::unique_ptr<ASTNode> parseUnary();
    std::unique_ptr<ASTNode> parsePrimary();
    
public:
    // 构造函数
    explicit Parser(const std::vector<Token>& tokens);
    
    // 解析方法
    std::unique_ptr<ProgramNode> parse();
    
    // 错误处理
    const std::vector<SyntaxError>& getErrors() const;
    bool hasErrors() const;
    void printErrors() const;
    
    // 重置解析器
    void reset(const std::vector<Token>& newTokens);
};

#endif // PARSER_H
