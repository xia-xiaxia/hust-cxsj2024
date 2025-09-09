#include "../include/Lexer.h"
#include <iostream>
#include <cctype>
#include <sstream>

// LexicalError类实现
LexicalError::LexicalError(const std::string& msg, int line, int column)
    : message(msg), line(line), column(column) {}

const char* LexicalError::what() const noexcept {
    return message.c_str();
}

std::string LexicalError::getFullMessage() const {
    std::ostringstream oss;
    oss << "Lexical error at " << line << ":" << column << ": " << message;
    return oss.str();
}

// Lexer类实现
Lexer::Lexer(const std::string& text) 
    : text(text), pos(0), line(1), column(1) {}

char Lexer::currentChar() const {
    if (pos >= text.length()) {
        return '\0';
    }
    return text[pos];
}

char Lexer::peekChar(int offset) const {
    size_t peekPos = pos + offset;
    if (peekPos >= text.length()) {
        return '\0';
    }
    return text[peekPos];
}

void Lexer::advance() {
    if (pos < text.length() && text[pos] == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    pos++;
}

void Lexer::skipWhitespace() {
    while (currentChar() != '\0' && std::isspace(currentChar()) && currentChar() != '\n') {
        advance();
    }
}

bool Lexer::skipComment() {
    // 单行注释 //
    if (currentChar() == '/' && peekChar() == '/') {
        while (currentChar() != '\0' && currentChar() != '\n') {
            advance();
        }
        return true;
    }
    
    // 多行注释 /* */
    if (currentChar() == '/' && peekChar() == '*') {
        advance(); // 跳过 /
        advance(); // 跳过 *
        
        while (currentChar() != '\0') {
            if (currentChar() == '*' && peekChar() == '/') {
                advance(); // 跳过 *
                advance(); // 跳过 /
                return true;
            }
            advance();
        }
        
        // 多行注释未闭合
        errors.emplace_back("Unterminated comment", line, column);
        return false;
    }
    
    return false;
}

Token Lexer::readNumber() {
    int startLine = line;
    int startColumn = column;
    std::string result;
    bool hasDot = false;
    
    while (currentChar() != '\0' && (std::isdigit(currentChar()) || currentChar() == '.')) {
        if (currentChar() == '.') {
            // 检查下一个字符，如果不是数字，则停止（可能是文件扩展名）
            if (!std::isdigit(peekChar())) {
                break;
            }
            if (hasDot) { // 第二个小数点
                break;
            }
            hasDot = true;
        }
        result += currentChar();
        advance();
    }
    
    if (result.back() == '.') {
        return createErrorToken("Invalid number format");
    }
    
    if (hasDot) {
        return Token(TokenType::FLOAT, result, startLine, startColumn);
    } else {
        return Token(TokenType::INTEGER, result, startLine, startColumn);
    }
}

Token Lexer::readIdentifier() {
    int startLine = line;
    int startColumn = column;
    std::string result;
    
    while (currentChar() != '\0' && 
           (std::isalnum(currentChar()) || currentChar() == '_')) {
        result += currentChar();
        advance();
    }
    
    // 检查是否为关键字
    TokenType tokenType = TokenTypeUtils::getKeywordType(result);
    return Token(tokenType, result, startLine, startColumn);
}

Token Lexer::readString() {
    int startLine = line;
    int startColumn = column;
    char quoteChar = currentChar(); // " 或 '
    advance(); // 跳过开始引号
    
    std::string result;
    while (currentChar() != '\0' && currentChar() != quoteChar) {
        if (currentChar() == '\\') {
            advance();
            if (currentChar() != '\0') {
                // 处理转义字符
                switch (currentChar()) {
                    case 'n': result += '\n'; break;
                    case 't': result += '\t'; break;
                    case 'r': result += '\r'; break;
                    case '\\': result += '\\'; break;
                    case '"': result += '"'; break;
                    case '\'': result += '\''; break;
                    default: result += currentChar(); break;
                }
                advance();
            }
        } else {
            result += currentChar();
            advance();
        }
    }
    
    if (currentChar() != quoteChar) {
        return createErrorToken("Unterminated string");
    }
    
    advance(); // 跳过结束引号
    return Token(TokenType::STRING, result, startLine, startColumn);
}

Token Lexer::createErrorToken(const std::string& message) {
    errors.emplace_back(message, line, column);
    return Token(TokenType::ERROR, std::string(1, currentChar()), line, column);
}

Token Lexer::getNextToken() {
    while (currentChar() != '\0') {
        int startLine = line;
        int startColumn = column;
        
        // 跳过空白字符
        if (std::isspace(currentChar())) {
            if (currentChar() == '\n') {
                char ch = currentChar();
                advance();
                return Token(TokenType::NEWLINE, std::string(1, ch), startLine, startColumn);
            } else {
                skipWhitespace();
                continue;
            }
        }
        
        // 跳过注释
        if (currentChar() == '/' && (peekChar() == '/' || peekChar() == '*')) {
            if (!skipComment()) {
                return createErrorToken("Unterminated comment");
            }
            continue;
        }
        
        // 数字
        if (std::isdigit(currentChar())) {
            return readNumber();
        }
        
        // 标识符和关键字
        if (std::isalpha(currentChar()) || currentChar() == '_') {
            return readIdentifier();
        }
        
        // 字符串
        if (currentChar() == '"' || currentChar() == '\'') {
            return readString();
        }
        
        // 双字符操作符
        char ch = currentChar();
        char nextCh = peekChar();
        std::string doubleChar = std::string(1, ch) + std::string(1, nextCh);
        
        if (doubleChar == "==") {
            advance(); advance();
            return Token(TokenType::EQ, doubleChar, startLine, startColumn);
        } else if (doubleChar == "!=") {
            advance(); advance();
            return Token(TokenType::NE, doubleChar, startLine, startColumn);
        } else if (doubleChar == "<=") {
            advance(); advance();
            return Token(TokenType::LE, doubleChar, startLine, startColumn);
        } else if (doubleChar == ">=") {
            advance(); advance();
            return Token(TokenType::GE, doubleChar, startLine, startColumn);
        } else if (doubleChar == "&&") {
            advance(); advance();
            return Token(TokenType::AND, doubleChar, startLine, startColumn);
        } else if (doubleChar == "||") {
            advance(); advance();
            return Token(TokenType::OR, doubleChar, startLine, startColumn);
        } else if (doubleChar == "++") {
            advance(); advance();
            return Token(TokenType::INCREMENT, doubleChar, startLine, startColumn);
        } else if (doubleChar == "--") {
            advance(); advance();
            return Token(TokenType::DECREMENT, doubleChar, startLine, startColumn);
        }
        
        // 单字符操作符和分隔符
        TokenType singleCharType;
        bool found = true;
        
        switch (ch) {
            case '+': singleCharType = TokenType::PLUS; break;
            case '-': singleCharType = TokenType::MINUS; break;
            case '*': singleCharType = TokenType::MULTIPLY; break;
            case '/': singleCharType = TokenType::DIVIDE; break;
            case '%': singleCharType = TokenType::MODULO; break;
            case '=': singleCharType = TokenType::ASSIGN; break;
            case '<': singleCharType = TokenType::LANGLE; break;
            case '>': singleCharType = TokenType::RANGLE; break;
            case '!': singleCharType = TokenType::NOT; break;
            case ';': singleCharType = TokenType::SEMICOLON; break;
            case ',': singleCharType = TokenType::COMMA; break;
            case '(': singleCharType = TokenType::LPAREN; break;
            case ')': singleCharType = TokenType::RPAREN; break;
            case '{': singleCharType = TokenType::LBRACE; break;
            case '}': singleCharType = TokenType::RBRACE; break;
            case '#': singleCharType = TokenType::HASH; break;
            case '.': singleCharType = TokenType::ERROR; break; // 点号单独出现时作为错误处理
            default: found = false; break;
        }
        
        if (found) {
            advance();
            return Token(singleCharType, std::string(1, ch), startLine, startColumn);
        }
        
        // 无法识别的字符
        advance();
        return createErrorToken("Unexpected character '" + std::string(1, ch) + "'");
    }
    
    // 文件结束
    return Token(TokenType::EOF_TOKEN, "", line, column);
}

std::vector<Token> Lexer::tokenize() {
    tokens.clear();
    errors.clear();
    
    while (true) {
        Token token = getNextToken();
        tokens.push_back(token);
        
        if (token.type == TokenType::EOF_TOKEN) {
            break;
        }
    }
    
    return tokens;
}

const std::vector<LexicalError>& Lexer::getErrors() const {
    return errors;
}

bool Lexer::hasErrors() const {
    return !errors.empty();
}

void Lexer::printErrors() const {
    for (const auto& error : errors) {
        std::cerr << error.getFullMessage() << std::endl;
    }
}

void Lexer::reset() {
    pos = 0;
    line = 1;
    column = 1;
    tokens.clear();
    errors.clear();
}

void Lexer::reset(const std::string& newText) {
    text = newText;
    reset();
}
