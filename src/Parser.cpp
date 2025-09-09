#include "../include/Parser.h"
#include <iostream>
#include <sstream>

// SyntaxError类实现
SyntaxError::SyntaxError(const std::string& msg, int line, int column)
    : message(msg), line(line), column(column) {}

const char* SyntaxError::what() const noexcept {
    return message.c_str();
}

std::string SyntaxError::getFullMessage() const {
    std::ostringstream oss;
    oss << "Syntax error at " << line << ":" << column << ": " << message;
    return oss.str();
}

// ASTNode基类实现
void ASTNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << toString() << std::endl;
}

// ProgramNode实现
std::string ProgramNode::toString() const {
    return "Program";
}

void ProgramNode::addStatement(std::unique_ptr<ASTNode> stmt) {
    statements.push_back(std::move(stmt));
}

// VarDeclarationNode实现
VarDeclarationNode::VarDeclarationNode(const std::string& type, const std::string& id)
    : type(type), identifier(id) {}

std::string VarDeclarationNode::toString() const {
    std::string result = "VarDeclaration: " + type + " " + identifier;
    if (initializer) {
        result += " = " + initializer->toString();
    }
    return result;
}

// AssignmentNode实现
AssignmentNode::AssignmentNode(const std::string& id) : identifier(id) {}

std::string AssignmentNode::toString() const {
    return "Assignment: " + identifier + " = " + 
           (expression ? expression->toString() : "null");
}

// BinaryExpressionNode实现
BinaryExpressionNode::BinaryExpressionNode(const std::string& op) : operator_(op) {}

std::string BinaryExpressionNode::toString() const {
    return "BinaryExpr: (" + 
           (left ? left->toString() : "null") + " " + operator_ + " " + 
           (right ? right->toString() : "null") + ")";
}

// UnaryExpressionNode实现
UnaryExpressionNode::UnaryExpressionNode(const std::string& op) : operator_(op) {}

std::string UnaryExpressionNode::toString() const {
    return "UnaryExpr: " + operator_ + 
           (operand ? operand->toString() : "null");
}

// LiteralNode实现
LiteralNode::LiteralNode(const std::string& val, TokenType t) : value(val), type(t) {}

std::string LiteralNode::toString() const {
    return "Literal: " + value + " (" + TokenTypeUtils::tokenTypeToString(type) + ")";
}

// IdentifierNode实现
IdentifierNode::IdentifierNode(const std::string& n) : name(n) {}

std::string IdentifierNode::toString() const {
    return "Identifier: " + name;
}

// IfStatementNode实现
std::string IfStatementNode::toString() const {
    std::string result = "IfStatement: condition(" + 
                        (condition ? condition->toString() : "null") + ")";
    if (thenStatement) {
        result += " then(" + thenStatement->toString() + ")";
    }
    if (elseStatement) {
        result += " else(" + elseStatement->toString() + ")";
    }
    return result;
}

// WhileStatementNode实现
std::string WhileStatementNode::toString() const {
    return "WhileStatement: condition(" + 
           (condition ? condition->toString() : "null") + ") body(" +
           (body ? body->toString() : "null") + ")";
}

// CompoundStatementNode实现
std::string CompoundStatementNode::toString() const {
    return "CompoundStatement {" + std::to_string(statements.size()) + " statements}";
}

void CompoundStatementNode::addStatement(std::unique_ptr<ASTNode> stmt) {
    statements.push_back(std::move(stmt));
}

// ReturnStatementNode实现
std::string ReturnStatementNode::toString() const {
    return "ReturnStatement: " + 
           (expression ? expression->toString() : "void");
}

// Parser类实现
Parser::Parser(const std::vector<Token>& tokens) 
    : tokens(tokens), currentToken(0) {}

const Token& Parser::getCurrentToken() const {
    if (currentToken >= tokens.size()) {
        static Token eofToken(TokenType::EOF_TOKEN, "", 0, 0);
        return eofToken;
    }
    return tokens[currentToken];
}

const Token& Parser::peekToken(int offset) const {
    size_t peekIndex = currentToken + offset;
    if (peekIndex >= tokens.size()) {
        static Token eofToken(TokenType::EOF_TOKEN, "", 0, 0);
        return eofToken;
    }
    return tokens[peekIndex];
}

bool Parser::isAtEnd() const {
    return getCurrentToken().type == TokenType::EOF_TOKEN;
}

void Parser::advance() {
    if (!isAtEnd()) {
        currentToken++;
    }
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) {
        return false;
    }
    return getCurrentToken().type == type;
}

Token Parser::consume(TokenType type, const std::string& errorMessage) {
    if (check(type)) {
        Token token = getCurrentToken();
        advance();
        return token;
    }
    
    recordError(errorMessage);
    return getCurrentToken();
}

void Parser::recordError(const std::string& message) {
    const Token& token = getCurrentToken();
    errors.emplace_back(message, token.line, token.column);
}

void Parser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (tokens[currentToken - 1].type == TokenType::SEMICOLON) {
            return;
        }
        
        switch (getCurrentToken().type) {
            case TokenType::INT:
            case TokenType::FLOAT_KW:
            case TokenType::CHAR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::FOR:
            case TokenType::RETURN:
                return;
            default:
                break;
        }
        
        advance();
    }
}

std::unique_ptr<ProgramNode> Parser::parseProgram() {
    auto program = std::make_unique<ProgramNode>();
    
    // 跳过换行符
    while (match(TokenType::NEWLINE)) {}
    
    while (!isAtEnd()) {
        size_t oldPos = currentToken; // 记录位置
        
        try {
            auto stmt = parseStatement();
            if (stmt) {
                program->addStatement(std::move(stmt));
            }
        } catch (const std::exception& e) {
            synchronize();
        }
        
        // 跳过换行符
        while (match(TokenType::NEWLINE)) {}
        
        // 防止死循环：如果位置没有前进，强制前进一个token
        if (currentToken == oldPos && !isAtEnd()) {
            recordError("Parser unable to process token, skipping");
            advance();
        }
    }
    
    return program;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    // 跳过换行符
    while (match(TokenType::NEWLINE)) {}
    
    // 检查是否到达文件末尾
    if (isAtEnd() || check(TokenType::RBRACE)) {
        return nullptr;
    }
    
    // 预处理指令（改进的处理）
    if (match(TokenType::HASH)) {
        if (match(TokenType::INCLUDE)) {
            // 处理 #include 指令
            if (match(TokenType::LANGLE)) {
                // #include <filename> 形式
                bool foundFilename = false;
                while (!check(TokenType::RANGLE) && !check(TokenType::NEWLINE) && !isAtEnd()) {
                    if (match(TokenType::IDENTIFIER) || match(TokenType::ERROR)) {
                        foundFilename = true;
                    } else {
                        advance(); // 跳过其他token（如点号等）
                    }
                }
                
                if (!foundFilename) {
                    recordError("Expected filename in #include directive");
                }
                
                if (!match(TokenType::RANGLE)) {
                    recordError("Expected '>' to close #include directive");
                }
            } else if (match(TokenType::STRING)) {
                // #include "filename" 形式 - 已经处理完毕
            } else {
                recordError("Expected '<filename>' or \"filename\" after #include");
            }
        } else {
            // 其他预处理指令，简单跳过
            while (!check(TokenType::NEWLINE) && !isAtEnd()) {
                advance();
            }
        }
        return nullptr; // 预处理指令不生成AST节点
    }
    
    // 函数定义或声明（简单处理：类型 标识符 左括号）
    if ((check(TokenType::INT) || check(TokenType::FLOAT_KW) || check(TokenType::CHAR) || check(TokenType::VOID)) 
        && peekToken().type == TokenType::IDENTIFIER && peekToken(2).type == TokenType::LPAREN) {
        // 简单跳过函数签名
        advance(); // 跳过返回类型
        advance(); // 跳过函数名
        advance(); // 跳过左括号
        
        // 跳过参数列表
        while (!check(TokenType::RPAREN) && !isAtEnd()) {
            advance();
        }
        consume(TokenType::RPAREN, "Expected ')' after function parameters");
        
        // 检查是函数声明还是定义
        if (check(TokenType::SEMICOLON)) {
            advance(); // 消费分号，这是函数声明
            return std::make_unique<LiteralNode>("function_declaration", TokenType::IDENTIFIER);
        } else {
            // 解析函数体（复合语句）
            return parseCompoundStatement();
        }
    }
    
    // 变量声明
    if (check(TokenType::INT) || check(TokenType::FLOAT_KW) || check(TokenType::CHAR)) {
        return parseVarDeclaration();
    }
    
    // if语句
    if (match(TokenType::IF)) {
        return parseIfStatement();
    }
    
    // while语句
    if (match(TokenType::WHILE)) {
        return parseWhileStatement();
    }
    
    // for语句
    if (match(TokenType::FOR)) {
        return parseForStatement();
    }
    
    // 复合语句
    if (check(TokenType::LBRACE)) {
        return parseCompoundStatement();
    }
    
    // return语句
    if (match(TokenType::RETURN)) {
        return parseReturnStatement();
    }
    
    // break语句（简单处理）
    if (match(TokenType::BREAK)) {
        consume(TokenType::SEMICOLON, "Expected ';' after break");
        return std::make_unique<LiteralNode>("break", TokenType::BREAK);
    }
    
    // continue语句（简单处理）
    if (match(TokenType::CONTINUE)) {
        consume(TokenType::SEMICOLON, "Expected ';' after continue");
        return std::make_unique<LiteralNode>("continue", TokenType::CONTINUE);
    }
    
    // 赋值或表达式语句
    if (check(TokenType::IDENTIFIER)) {
        // 向前看，判断是赋值、函数调用还是表达式
        if (peekToken().type == TokenType::ASSIGN) {
            return parseAssignment();
        } else if (peekToken().type == TokenType::LPAREN) {
            // 函数调用
            return parseExpressionStatement();
        } else if (peekToken().type == TokenType::IDENTIFIER && peekToken().value == "++") {
            // i++ 语句
            return parseExpressionStatement();
        }
    }
    
    // 表达式语句
    return parseExpressionStatement();
}

std::unique_ptr<ASTNode> Parser::parseVarDeclaration() {
    std::string type = getCurrentToken().value;
    advance(); // 消费类型token
    
    Token identifier = consume(TokenType::IDENTIFIER, "Expected variable name");
    auto varDecl = std::make_unique<VarDeclarationNode>(type, identifier.value);
    
    // 检查是否有初始化
    if (match(TokenType::ASSIGN)) {
        varDecl->initializer = parseExpression();
    }
    
    // 处理多个变量声明（简单跳过逗号后的变量）
    while (match(TokenType::COMMA)) {
        consume(TokenType::IDENTIFIER, "Expected variable name after ','");
        if (match(TokenType::ASSIGN)) {
            parseExpression(); // 跳过初始化表达式
        }
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
    return std::move(varDecl);
}

std::unique_ptr<ASTNode> Parser::parseAssignment() {
    Token identifier = consume(TokenType::IDENTIFIER, "Expected identifier");
    auto assignment = std::make_unique<AssignmentNode>(identifier.value);
    
    consume(TokenType::ASSIGN, "Expected '='");
    assignment->expression = parseExpression();
    
    consume(TokenType::SEMICOLON, "Expected ';' after assignment");
    return std::move(assignment);
}

std::unique_ptr<ASTNode> Parser::parseIfStatement() {
    auto ifStmt = std::make_unique<IfStatementNode>();
    
    consume(TokenType::LPAREN, "Expected '(' after 'if'");
    ifStmt->condition = parseExpression();
    consume(TokenType::RPAREN, "Expected ')' after if condition");
    
    ifStmt->thenStatement = parseStatement();
    
    // 跳过换行符，以便检查else
    while (match(TokenType::NEWLINE)) {}
    
    if (match(TokenType::ELSE)) {
        ifStmt->elseStatement = parseStatement();
    }
    
    return std::move(ifStmt);
}

std::unique_ptr<ASTNode> Parser::parseWhileStatement() {
    auto whileStmt = std::make_unique<WhileStatementNode>();
    
    consume(TokenType::LPAREN, "Expected '(' after 'while'");
    whileStmt->condition = parseExpression();
    consume(TokenType::RPAREN, "Expected ')' after while condition");
    
    whileStmt->body = parseStatement();
    
    return std::move(whileStmt);
}

std::unique_ptr<ASTNode> Parser::parseForStatement() {
    auto forStmt = std::make_unique<WhileStatementNode>(); // 简单用while节点代替
    
    consume(TokenType::LPAREN, "Expected '(' after 'for'");
    
    // 跳过初始化部分
    while (!check(TokenType::SEMICOLON) && !isAtEnd()) {
        advance();
    }
    consume(TokenType::SEMICOLON, "Expected ';' after for loop initialization");
    
    // 跳过条件部分
    while (!check(TokenType::SEMICOLON) && !isAtEnd()) {
        advance();
    }
    consume(TokenType::SEMICOLON, "Expected ';' after for loop condition");
    
    // 跳过更新部分
    while (!check(TokenType::RPAREN) && !isAtEnd()) {
        advance();
    }
    consume(TokenType::RPAREN, "Expected ')' after for loop");
    
    // 解析循环体
    forStmt->body = parseStatement();
    
    return std::move(forStmt);
}

std::unique_ptr<ASTNode> Parser::parseCompoundStatement() {
    auto compound = std::make_unique<CompoundStatementNode>();
    
    consume(TokenType::LBRACE, "Expected '{'");
    
    // 跳过换行符
    while (match(TokenType::NEWLINE)) {}
    
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        size_t oldPos = currentToken; // 记录位置防止死循环
        
        auto stmt = parseStatement();
        if (stmt) {
            compound->addStatement(std::move(stmt));
        }
        
        // 跳过换行符
        while (match(TokenType::NEWLINE)) {}
        
        // 防止死循环：如果位置没有前进，强制前进一个token
        if (currentToken == oldPos && !isAtEnd() && !check(TokenType::RBRACE)) {
            recordError("Parser stuck, skipping token");
            advance();
        }
    }
    
    consume(TokenType::RBRACE, "Expected '}'");
    return std::move(compound);
}

std::unique_ptr<ASTNode> Parser::parseReturnStatement() {
    auto returnStmt = std::make_unique<ReturnStatementNode>();
    
    if (!check(TokenType::SEMICOLON)) {
        returnStmt->expression = parseExpression();
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after return statement");
    return std::move(returnStmt);
}

std::unique_ptr<ASTNode> Parser::parseExpressionStatement() {
    // 如果遇到右大括号或到达文件末尾，返回null
    if (check(TokenType::RBRACE) || isAtEnd()) {
        return nullptr;
    }
    
    // 如果遇到意外的token，尝试恢复
    if (check(TokenType::SEMICOLON)) {
        advance(); // 跳过多余的分号
        return nullptr;
    }
    
    // 处理独立的INCREMENT/DECREMENT语句 (前缀)
    if (check(TokenType::INCREMENT) || check(TokenType::DECREMENT)) {
        TokenType opType = getCurrentToken().type;
        advance(); // 消费++或--
        
        if (check(TokenType::IDENTIFIER)) {
            std::string varName = getCurrentToken().value;
            advance(); // 消费标识符
            consume(TokenType::SEMICOLON, "Expected ';' after increment/decrement");
            std::string op = (opType == TokenType::INCREMENT) ? "++" : "--";
            return std::make_unique<IdentifierNode>(op + varName);
        } else {
            std::string opName = (opType == TokenType::INCREMENT) ? "increment" : "decrement";
            std::string msg = "Expected identifier after " + opName + " operator";
            recordError(msg);
            return nullptr;
        }
    }
    
    try {
        auto expr = parseExpression();
        if (expr) {
            consume(TokenType::SEMICOLON, "Expected ';' after expression");
        }
        return expr;
    } catch (const std::exception& e) {
        // 错误恢复：跳到下一个分号或右大括号
        while (!isAtEnd() && !check(TokenType::SEMICOLON) && !check(TokenType::RBRACE)) {
            advance();
        }
        if (match(TokenType::SEMICOLON)) {
            // 跳过分号
        }
        return nullptr;
    }
}

std::unique_ptr<ASTNode> Parser::parseExpression() {
    return parseLogicalOr();
}

std::unique_ptr<ASTNode> Parser::parseLogicalOr() {
    auto expr = parseLogicalAnd();
    
    while (match(TokenType::OR)) {
        std::string operator_ = tokens[currentToken - 1].value;
        auto right = parseLogicalAnd();
        auto binaryExpr = std::make_unique<BinaryExpressionNode>(operator_);
        binaryExpr->left = std::move(expr);
        binaryExpr->right = std::move(right);
        expr = std::move(binaryExpr);
    }
    
    return expr;
}

std::unique_ptr<ASTNode> Parser::parseLogicalAnd() {
    auto expr = parseEquality();
    
    while (match(TokenType::AND)) {
        std::string operator_ = tokens[currentToken - 1].value;
        auto right = parseEquality();
        auto binaryExpr = std::make_unique<BinaryExpressionNode>(operator_);
        binaryExpr->left = std::move(expr);
        binaryExpr->right = std::move(right);
        expr = std::move(binaryExpr);
    }
    
    return expr;
}

std::unique_ptr<ASTNode> Parser::parseEquality() {
    auto expr = parseComparison();
    
    while (match(TokenType::EQ) || match(TokenType::NE)) {
        std::string operator_ = tokens[currentToken - 1].value;
        auto right = parseComparison();
        auto binaryExpr = std::make_unique<BinaryExpressionNode>(operator_);
        binaryExpr->left = std::move(expr);
        binaryExpr->right = std::move(right);
        expr = std::move(binaryExpr);
    }
    
    return expr;
}

std::unique_ptr<ASTNode> Parser::parseComparison() {
    auto expr = parseAddition();
    
    while (match(TokenType::RANGLE) || match(TokenType::GE) || 
           match(TokenType::LANGLE) || match(TokenType::LE)) {
        std::string operator_ = tokens[currentToken - 1].value;
        auto right = parseAddition();
        auto binaryExpr = std::make_unique<BinaryExpressionNode>(operator_);
        binaryExpr->left = std::move(expr);
        binaryExpr->right = std::move(right);
        expr = std::move(binaryExpr);
    }
    
    return expr;
}

std::unique_ptr<ASTNode> Parser::parseAddition() {
    auto expr = parseMultiplication();
    
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        std::string operator_ = tokens[currentToken - 1].value;
        auto right = parseMultiplication();
        auto binaryExpr = std::make_unique<BinaryExpressionNode>(operator_);
        binaryExpr->left = std::move(expr);
        binaryExpr->right = std::move(right);
        expr = std::move(binaryExpr);
    }
    
    return expr;
}

std::unique_ptr<ASTNode> Parser::parseMultiplication() {
    auto expr = parseUnary();
    
    while (match(TokenType::MULTIPLY) || match(TokenType::DIVIDE) || match(TokenType::MODULO)) {
        std::string operator_ = tokens[currentToken - 1].value;
        auto right = parseUnary();
        auto binaryExpr = std::make_unique<BinaryExpressionNode>(operator_);
        binaryExpr->left = std::move(expr);
        binaryExpr->right = std::move(right);
        expr = std::move(binaryExpr);
    }
    
    return expr;
}

std::unique_ptr<ASTNode> Parser::parseUnary() {
    if (match(TokenType::NOT) || match(TokenType::MINUS)) {
        std::string operator_ = tokens[currentToken - 1].value;
        auto operand = parseUnary();
        auto unaryExpr = std::make_unique<UnaryExpressionNode>(operator_);
        unaryExpr->operand = std::move(operand);
        return std::move(unaryExpr);
    }
    
    return parsePrimary();
}

std::unique_ptr<ASTNode> Parser::parsePrimary() {
    // 数字字面量
    if (match(TokenType::INTEGER) || match(TokenType::FLOAT)) {
        Token token = tokens[currentToken - 1];
        return std::make_unique<LiteralNode>(token.value, token.type);
    }
    
    // 字符串字面量
    if (match(TokenType::STRING)) {
        Token token = tokens[currentToken - 1];
        return std::make_unique<LiteralNode>(token.value, token.type);
    }
    
    // 标识符
    if (match(TokenType::IDENTIFIER)) {
        Token token = tokens[currentToken - 1];
        
        // 检查是否有函数调用
        if (check(TokenType::LPAREN)) {
            advance(); // 消费左括号
            
            // 简单跳过参数
            while (!check(TokenType::RPAREN) && !isAtEnd()) {
                parseExpression(); // 解析参数
                if (match(TokenType::COMMA)) {
                    continue; // 多个参数
                }
                break;
            }
            consume(TokenType::RPAREN, "Expected ')' after function arguments");
            return std::make_unique<IdentifierNode>(token.value + "()");
        }
        
        // 检查是否有后缀++
        if (check(TokenType::INCREMENT)) {
            advance(); // 消费++
            return std::make_unique<IdentifierNode>(token.value + "++");
        }
        
        return std::make_unique<IdentifierNode>(token.value);
    }
    
    // 括号表达式
    if (match(TokenType::LPAREN)) {
        auto expr = parseExpression();
        consume(TokenType::RPAREN, "Expected ')' after expression");
        return expr;
    }
    
    // 如果遇到意外token，记录错误但不抛出异常
    recordError("Expected expression");
    return std::make_unique<LiteralNode>("ERROR", TokenType::ERROR);
}

std::unique_ptr<ProgramNode> Parser::parse() {
    errors.clear();
    currentToken = 0;
    
    return parseProgram();
}

const std::vector<SyntaxError>& Parser::getErrors() const {
    return errors;
}

bool Parser::hasErrors() const {
    return !errors.empty();
}

void Parser::printErrors() const {
    for (const auto& error : errors) {
        std::cerr << error.getFullMessage() << std::endl;
    }
}

void Parser::reset(const std::vector<Token>& newTokens) {
    tokens = newTokens;
    currentToken = 0;
    errors.clear();
}
