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

void ASTNode::printChinese(int indent) const {
    // 默认实现，子类可以重写来提供更详细的中文输出
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << toString() << std::endl;
}

// ProgramNode实现
std::string ProgramNode::toString() const {
    return "Program";
}

void ProgramNode::printChinese(int indent) const {
    // 直接打印所有语句，不需要额外的"Program"标识
    for (const auto& stmt : statements) {
        if (stmt) {
            stmt->printChinese(indent);
        }
    }
}

void ProgramNode::addStatement(std::unique_ptr<ASTNode> stmt) {
    statements.push_back(std::move(stmt));
}

// VarDeclarationNode实现
VarDeclarationNode::VarDeclarationNode(const std::string& type, const std::string& id)
    : type(type), identifier(id) {}

std::string VarDeclarationNode::toString() const {
    return "变量声明: " + type;
}

void VarDeclarationNode::printChinese(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "变量声明: " << type << std::endl;
    
    // 打印标识符
    for (int i = 0; i < indent + 1; ++i) {
        std::cout << "  ";
    }
    std::cout << "标识符: " << identifier << std::endl;
    
    // 如果有初始化表达式，打印运算符=和值
    if (initializer) {
        for (int i = 0; i < indent + 2; ++i) {
            std::cout << "  ";
        }
        std::cout << "运算符: =" << std::endl;
        initializer->printChinese(indent + 3);
    }
}

// AssignmentNode实现
AssignmentNode::AssignmentNode(const std::string& id) : identifier(id) {}

std::string AssignmentNode::toString() const {
    return "赋值: " + identifier;
}

void AssignmentNode::printChinese(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "标识符: " << identifier << std::endl;
    
    if (expression) {
        expression->printChinese(indent);
    }
}

// BinaryExpressionNode实现
BinaryExpressionNode::BinaryExpressionNode(const std::string& op) : operator_(op) {}

std::string BinaryExpressionNode::toString() const {
    return "运算符: " + operator_;
}

void BinaryExpressionNode::printChinese(int indent) const {
    if (left) {
        left->printChinese(indent);
    }
    
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "运算符: " << operator_ << std::endl;
    
    if (right) {
        right->printChinese(indent + 1);
    }
}

// UnaryExpressionNode实现
UnaryExpressionNode::UnaryExpressionNode(const std::string& op) : operator_(op) {}

std::string UnaryExpressionNode::toString() const {
    return "运算符: " + operator_;
}

void UnaryExpressionNode::printChinese(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "运算符: " << operator_ << std::endl;
    
    if (operand) {
        operand->printChinese(indent);
    }
}

// LiteralNode实现
LiteralNode::LiteralNode(const std::string& val, TokenType t) : value(val), type(t) {}

std::string LiteralNode::toString() const {
    if (type == TokenType::INTEGER || type == TokenType::FLOAT) {
        return "数字: " + value;
    } else if (type == TokenType::STRING) {
        return "字符串: " + value;
    } else {
        return value;
    }
}

void LiteralNode::printChinese(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    
    if (type == TokenType::INTEGER || type == TokenType::FLOAT) {
        std::cout << "数字: " << value << std::endl;
    } else if (type == TokenType::STRING) {
        std::cout << "字符串: " << value << std::endl;
    } else if (type == TokenType::BREAK) {
        std::cout << "break语句: break" << std::endl;
    } else if (type == TokenType::CONTINUE) {
        std::cout << "continue语句: continue" << std::endl;
    } else {
        std::cout << value << std::endl;
    }
}

// IdentifierNode实现
IdentifierNode::IdentifierNode(const std::string& n) : name(n) {}

std::string IdentifierNode::toString() const {
    return "标识符: " + name;
}

void IdentifierNode::printChinese(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "标识符: " << name << std::endl;
}

// IfStatementNode实现
std::string IfStatementNode::toString() const {
    return "if语句: if";
}

void IfStatementNode::printChinese(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "if语句: if" << std::endl;
    
    // 打印条件
    for (int i = 0; i < indent + 1; ++i) {
        std::cout << "  ";
    }
    std::cout << "表达式:" << std::endl;
    
    if (condition) {
        condition->printChinese(indent + 2);
    }
    
    // 打印then语句
    if (thenStatement) {
        thenStatement->printChinese(indent + 1);
    }
    
    // 打印else语句
    if (elseStatement) {
        for (int i = 0; i < indent + 1; ++i) {
            std::cout << "  ";
        }
        std::cout << "关键字: else" << std::endl;
        elseStatement->printChinese(indent + 2);
    }
}

// WhileStatementNode实现
std::string WhileStatementNode::toString() const {
    return "while语句: while";
}

void WhileStatementNode::printChinese(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "while语句: while" << std::endl;
    
    // 打印条件
    for (int i = 0; i < indent + 1; ++i) {
        std::cout << "  ";
    }
    std::cout << "表达式:" << std::endl;
    
    if (condition) {
        condition->printChinese(indent + 2);
    }
    
    // 打印循环体
    if (body) {
        body->printChinese(indent + 1);
    }
}

// CompoundStatementNode实现
std::string CompoundStatementNode::toString() const {
    return "复合语句:";
}

void CompoundStatementNode::printChinese(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "复合语句:" << std::endl;
    
    // 打印所有子语句
    for (const auto& stmt : statements) {
        if (stmt) {
            stmt->printChinese(indent + 1);
        }
    }
}

void CompoundStatementNode::addStatement(std::unique_ptr<ASTNode> stmt) {
    statements.push_back(std::move(stmt));
}

// ReturnStatementNode实现
std::string ReturnStatementNode::toString() const {
    return "return语句: return";
}

void ReturnStatementNode::printChinese(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "return语句: return" << std::endl;
    
    if (expression) {
        expression->printChinese(indent + 1);
    }
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
            std::string content = "";
            if (match(TokenType::LANGLE)) {
                // #include <filename> 形式
                while (!check(TokenType::RANGLE) && !check(TokenType::NEWLINE) && !isAtEnd()) {
                    content += getCurrentToken().value;
                    advance();
                }
                
                if (match(TokenType::RANGLE)) {
                    content = "<" + content + ">";
                } else {
                    recordError("Expected '>' to close #include directive");
                }
            } else if (match(TokenType::STRING)) {
                // #include "filename" 形式
                content = tokens[currentToken - 1].value;
            } else {
                recordError("Expected '<filename>' or \"filename\" after #include");
            }
            
            return std::make_unique<PreprocessorDirectiveNode>("include", content);
        } else if (match(TokenType::DEFINE)) {
            // 处理 #define 指令
            std::string content = "";
            while (!check(TokenType::NEWLINE) && !isAtEnd()) {
                if (!content.empty()) content += " ";
                content += getCurrentToken().value;
                advance();
            }
            return std::make_unique<PreprocessorDirectiveNode>("define", content);
        } else {
            // 其他预处理指令
            std::string content = "";
            while (!check(TokenType::NEWLINE) && !isAtEnd()) {
                if (!content.empty()) content += " ";
                content += getCurrentToken().value;
                advance();
            }
            return std::make_unique<PreprocessorDirectiveNode>("unknown", content);
        }
    }
    
    // 函数定义或声明（简单处理：类型 标识符 左括号）
    if ((check(TokenType::INT) || check(TokenType::FLOAT_KW) || check(TokenType::CHAR) || check(TokenType::VOID)) 
        && peekToken().type == TokenType::IDENTIFIER && peekToken(2).type == TokenType::LPAREN) {
        
        // 获取返回类型和函数名
        std::string returnType = getCurrentToken().value;
        advance(); // 跳过返回类型
        
        std::string functionName = getCurrentToken().value;
        advance(); // 跳过函数名
        advance(); // 跳过左括号
        
        // 创建函数节点（暂时简化参数处理）
        auto funcNode = std::make_unique<FunctionDeclarationNode>(returnType, functionName);
        
        // 跳过参数列表
        while (!check(TokenType::RPAREN) && !isAtEnd()) {
            // 简单解析参数（类型 + 标识符）
            if (check(TokenType::INT) || check(TokenType::FLOAT_KW) || check(TokenType::CHAR) || check(TokenType::VOID)) {
                std::string paramType = getCurrentToken().value;
                advance();
                if (check(TokenType::IDENTIFIER)) {
                    std::string paramName = getCurrentToken().value;
                    advance();
                    funcNode->parameters.push_back(
                        std::make_unique<VarDeclarationNode>(paramType, paramName)
                    );
                }
            } else if (check(TokenType::IDENTIFIER)) {
                // 如果遇到标识符作为类型，这可能是错误的类型名
                std::string invalidType = getCurrentToken().value;
                recordError("Unknown type '" + invalidType + "' in function parameter");
                advance(); // 跳过错误的类型
                if (check(TokenType::IDENTIFIER)) {
                    advance(); // 跳过参数名
                }
            } else {
                advance(); // 跳过其他token
            }
            if (match(TokenType::COMMA)) {
                // 继续下一个参数
            }
        }
        consume(TokenType::RPAREN, "Expected ')' after function parameters");
        
        // 检查是函数声明还是定义
        if (check(TokenType::SEMICOLON)) {
            advance(); // 消费分号，这是函数声明
            return std::move(funcNode);
        } else {
            // 这是函数定义，需要解析函数体
            auto funcDefNode = std::make_unique<FunctionDefinitionNode>(returnType, functionName);
            funcDefNode->parameters = std::move(funcNode->parameters);
            funcDefNode->body = parseCompoundStatement();
            return std::move(funcDefNode);
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
        return std::make_unique<BreakStatementNode>();
    }
    
    // continue语句（简单处理）
    if (match(TokenType::CONTINUE)) {
        consume(TokenType::SEMICOLON, "Expected ';' after continue");
        return std::make_unique<ContinueStatementNode>();
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
    
    consume(TokenType::ASSIGN, "Expected '='");
    auto expression = parseExpression();
    
    // 创建二元表达式节点表示赋值
    auto assignment = std::make_unique<BinaryExpressionNode>("=");
    assignment->left = std::make_unique<IdentifierNode>(identifier.value);
    assignment->right = std::move(expression);
    
    consume(TokenType::SEMICOLON, "Expected ';' after assignment");
    
    // 包装在表达式语句中
    return std::make_unique<ExpressionStatementNode>(std::move(assignment));
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
    auto forStmt = std::make_unique<ForStatementNode>();
    
    consume(TokenType::LPAREN, "Expected '(' after 'for'");
    
    // 解析初始化部分（通常是变量声明）
    if (check(TokenType::INT) || check(TokenType::FLOAT_KW) || check(TokenType::CHAR)) {
        forStmt->initialization = parseVarDeclaration();
    } else if (!check(TokenType::SEMICOLON)) {
        forStmt->initialization = parseExpression();
        consume(TokenType::SEMICOLON, "Expected ';' after for loop initialization");
    } else {
        consume(TokenType::SEMICOLON, "Expected ';' after for loop initialization");
    }
    
    // 解析条件部分
    if (!check(TokenType::SEMICOLON)) {
        forStmt->condition = parseExpression();
    }
    consume(TokenType::SEMICOLON, "Expected ';' after for loop condition");
    
    // 解析更新部分
    if (!check(TokenType::RPAREN)) {
        forStmt->update = parseExpression();
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
            return std::make_unique<ExpressionStatementNode>(std::move(expr));
        }
        return nullptr;
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
            
            auto funcCall = std::make_unique<FunctionCallNode>(token.value);
            
            // 解析参数
            while (!check(TokenType::RPAREN) && !isAtEnd()) {
                auto arg = parseExpression();
                if (arg) {
                    funcCall->arguments.push_back(std::move(arg));
                }
                if (match(TokenType::COMMA)) {
                    continue; // 多个参数
                }
                break;
            }
            consume(TokenType::RPAREN, "Expected ')' after function arguments");
            return std::move(funcCall);
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

// 新添加的节点类实现

// PreprocessorDirectiveNode实现
PreprocessorDirectiveNode::PreprocessorDirectiveNode(const std::string& dir, const std::string& cont)
    : directive(dir), content(cont) {}

std::string PreprocessorDirectiveNode::toString() const {
    return "预处理指令: # " + directive + " " + content;
}

void PreprocessorDirectiveNode::printChinese(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "预处理指令: # " << directive << " " << content << std::endl;
}

// FunctionDeclarationNode实现
FunctionDeclarationNode::FunctionDeclarationNode(const std::string& retType, const std::string& funcName)
    : returnType(retType), name(funcName) {}

std::string FunctionDeclarationNode::toString() const {
    return "函数声明: " + returnType;
}

void FunctionDeclarationNode::printChinese(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "函数声明: " << returnType << std::endl;
    
    for (int i = 0; i < indent + 1; ++i) {
        std::cout << "  ";
    }
    std::cout << "标识符: " << name << std::endl;
    
    // 打印参数
    for (const auto& param : parameters) {
        if (param) {
            param->printChinese(indent + 1);
        }
    }
}

// FunctionDefinitionNode实现
FunctionDefinitionNode::FunctionDefinitionNode(const std::string& retType, const std::string& funcName)
    : returnType(retType), name(funcName) {}

std::string FunctionDefinitionNode::toString() const {
    return "函数定义: " + returnType;
}

void FunctionDefinitionNode::printChinese(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "函数定义: " << returnType << std::endl;
    
    for (int i = 0; i < indent + 1; ++i) {
        std::cout << "  ";
    }
    std::cout << "标识符: " << name << std::endl;
    
    // 打印参数
    for (const auto& param : parameters) {
        if (param) {
            param->printChinese(indent + 1);
        }
    }
    
    // 打印函数体
    if (body) {
        body->printChinese(indent + 1);
    }
}

// ExpressionStatementNode实现
ExpressionStatementNode::ExpressionStatementNode(std::unique_ptr<ASTNode> expr)
    : expression(std::move(expr)) {}

std::string ExpressionStatementNode::toString() const {
    return "表达式语句:";
}

void ExpressionStatementNode::printChinese(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "表达式语句:" << std::endl;
    
    if (expression) {
        for (int i = 0; i < indent + 1; ++i) {
            std::cout << "  ";
        }
        std::cout << "表达式:" << std::endl;
        expression->printChinese(indent + 2);
    }
}

// FunctionCallNode实现
FunctionCallNode::FunctionCallNode(const std::string& funcName) : name(funcName) {}

std::string FunctionCallNode::toString() const {
    return "函数调用: " + name;
}

void FunctionCallNode::printChinese(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "函数调用: " << name << std::endl;
    
    // 打印参数
    for (const auto& arg : arguments) {
        if (arg) {
            arg->printChinese(indent + 1);
        }
    }
}

// ForStatementNode实现
std::string ForStatementNode::toString() const {
    return "for语句: for";
}

void ForStatementNode::printChinese(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "for语句: for" << std::endl;
    
    // 打印初始化
    if (initialization) {
        initialization->printChinese(indent + 1);
    }
    
    // 打印条件
    if (condition) {
        for (int i = 0; i < indent + 1; ++i) {
            std::cout << "  ";
        }
        std::cout << "表达式:" << std::endl;
        condition->printChinese(indent + 2);
    }
    
    // 打印更新表达式
    if (update) {
        for (int i = 0; i < indent + 1; ++i) {
            std::cout << "  ";
        }
        std::cout << "表达式:" << std::endl;
        update->printChinese(indent + 2);
    }
    
    // 打印循环体
    if (body) {
        body->printChinese(indent + 1);
    }
}

// BreakStatementNode实现
std::string BreakStatementNode::toString() const {
    return "break语句: break";
}

void BreakStatementNode::printChinese(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "break语句: break" << std::endl;
}

// ContinueStatementNode实现
std::string ContinueStatementNode::toString() const {
    return "continue语句: continue";
}

void ContinueStatementNode::printChinese(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "continue语句: continue" << std::endl;
}
