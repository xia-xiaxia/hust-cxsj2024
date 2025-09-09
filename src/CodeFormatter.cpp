#include "CodeFormatter.h"
#include <sstream>
#include <iostream>

CodeFormatter::CodeFormatter() : currentToken(0), indentLevel(0) {}

void CodeFormatter::setIndentSize(int size) {
    // 这里可以修改INDENT_SIZE，但为了简单起见，我们保持常量
}

std::string CodeFormatter::getIndent() const {
    return std::string(indentLevel * INDENT_SIZE, ' ');
}

std::string CodeFormatter::formatToken(const Token& token) const {
    // 对某些token进行特殊格式化
    switch (token.type) {
        case TokenType::LBRACE:
            return " {";
        case TokenType::RBRACE:
            return "}";
        case TokenType::SEMICOLON:
            return ";";
        case TokenType::COMMA:
            return ", ";
        default:
            return token.value;
    }
}

bool CodeFormatter::needsSpaceBefore(const Token& current, const Token& previous) const {
    // 在这些token之前需要空格
    switch (current.type) {
        case TokenType::LBRACE:
            return true;
        case TokenType::ELSE:
            return true;
        case TokenType::IDENTIFIER:
            // 如果前一个是关键字或标识符，需要空格
            return (previous.type == TokenType::INT || 
                    previous.type == TokenType::VOID ||
                    previous.type == TokenType::FLOAT_KW ||
                    previous.type == TokenType::CHAR ||
                    previous.type == TokenType::IF ||
                    previous.type == TokenType::WHILE ||
                    previous.type == TokenType::FOR ||
                    previous.type == TokenType::RETURN ||
                    previous.type == TokenType::IDENTIFIER);
        case TokenType::ASSIGN:
        case TokenType::EQ:
        case TokenType::NE:
        case TokenType::LT:
        case TokenType::LE:
        case TokenType::GT:
        case TokenType::GE:
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::MULTIPLY:
        case TokenType::DIVIDE:
        case TokenType::AND:
        case TokenType::OR:
            return true;
        default:
            return false;
    }
}

bool CodeFormatter::needsSpaceAfter(const Token& current, const Token& next) const {
    // 在这些token之后需要空格
    switch (current.type) {
        case TokenType::INT:
        case TokenType::VOID:
        case TokenType::FLOAT_KW:
        case TokenType::CHAR:
        case TokenType::IF:
        case TokenType::WHILE:
        case TokenType::FOR:
        case TokenType::RETURN:
        case TokenType::ELSE:
            return true;
        case TokenType::ASSIGN:
        case TokenType::EQ:
        case TokenType::NE:
        case TokenType::LT:
        case TokenType::LE:
        case TokenType::GT:
        case TokenType::GE:
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::MULTIPLY:
        case TokenType::DIVIDE:
        case TokenType::AND:
        case TokenType::OR:
            return true;
        case TokenType::COMMA:
            return true;
        case TokenType::RPAREN:
            return next.type == TokenType::LBRACE;
        case TokenType::HASH:
            return next.type == TokenType::INCLUDE || next.type == TokenType::DEFINE;
        case TokenType::INCLUDE:
        case TokenType::DEFINE:
            return true;
        default:
            return false;
    }
}

bool CodeFormatter::shouldNewlineBefore(const Token& current, const Token& previous) const {
    // 在这些token之前需要换行
    switch (current.type) {
        case TokenType::RBRACE:
            return true;
        case TokenType::ELSE:
            return true;
        default:
            return false;
    }
}

bool CodeFormatter::shouldNewlineAfter(const Token& current, const Token& next) const {
    // 在这些token之后需要换行
    switch (current.type) {
        case TokenType::SEMICOLON:
            return next.type != TokenType::RBRACE && next.type != TokenType::EOF_TOKEN;
        case TokenType::LBRACE:
            return true;
        case TokenType::RBRACE:
            return next.type != TokenType::EOF_TOKEN && 
                   next.type != TokenType::ELSE &&
                   next.type != TokenType::SEMICOLON;
        // 预处理指令后需要换行
        case TokenType::RANGLE:
            // 如果前面有#include，需要换行
            return true;
        case TokenType::FLOAT:
        case TokenType::INTEGER:
            // 如果是在#define之后，需要换行
            return true;
        default:
            return false;
    }
}

bool CodeFormatter::shouldIndentIncrease(const Token& token) const {
    return token.type == TokenType::LBRACE;
}

bool CodeFormatter::shouldIndentDecrease(const Token& token) const {
    return token.type == TokenType::RBRACE;
}

std::string CodeFormatter::format(const std::vector<Token>& inputTokens) {
    tokens = inputTokens;
    currentToken = 0;
    indentLevel = 0;
    
    std::ostringstream formatted;
    bool atLineStart = true; // 是否在行首
    int forDepth = 0; // 当前for循环的深度（用于跟踪for循环的括号）
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        Token& current = tokens[i];
        
        // 跳过换行符和空白符token，我们自己控制格式
        if (current.type == TokenType::NEWLINE || current.type == TokenType::WHITESPACE) {
            continue;
        }
        
        // EOF token，直接结束
        if (current.type == TokenType::EOF_TOKEN) {
            break;
        }
        
        // 跟踪for循环状态
        if (current.type == TokenType::FOR) {
            forDepth = 1; // 开始for循环
        } else if (forDepth > 0) {
            if (current.type == TokenType::LPAREN) {
                forDepth = 2; // 进入for循环的括号内
            } else if (current.type == TokenType::RPAREN && forDepth == 2) {
                forDepth = 0; // 退出for循环的括号
            }
        }
        
        // 处理右大括号的缩进减少
        if (current.type == TokenType::RBRACE) {
            indentLevel--;
        }
        
        // 添加缩进（只在行首）
        if (atLineStart) {
            formatted << getIndent();
            atLineStart = false;
        }
        
        // 特殊情况：else前需要换行和空格
        if (current.type == TokenType::ELSE) {
            formatted << "\n" << getIndent() << "else ";
            continue; // 跳过后面的处理
        }
        
        // 检查是否需要在当前token前加空格
        if (i > 0) {
            Token& prev = tokens[i-1];
            if (prev.type != TokenType::NEWLINE && prev.type != TokenType::WHITESPACE) {
                bool needSpaceBefore = false;
                
                // 二元操作符前需要空格
                if (current.type == TokenType::ASSIGN || current.type == TokenType::EQ ||
                    current.type == TokenType::NE || current.type == TokenType::LT ||
                    current.type == TokenType::LE || current.type == TokenType::GT ||
                    current.type == TokenType::GE || current.type == TokenType::PLUS ||
                    current.type == TokenType::MINUS || current.type == TokenType::MULTIPLY ||
                    current.type == TokenType::DIVIDE || current.type == TokenType::AND ||
                    current.type == TokenType::OR || 
                    (current.type == TokenType::RANGLE && prev.type != TokenType::IDENTIFIER)) {
                    // 但是如果前面是左括号，不需要空格
                    if (prev.type != TokenType::LPAREN) {
                        needSpaceBefore = true;
                    }
                }
                // 标识符前需要空格的情况
                else if (current.type == TokenType::IDENTIFIER) {
                    if (prev.type == TokenType::INT || prev.type == TokenType::VOID ||
                        prev.type == TokenType::FLOAT_KW || prev.type == TokenType::CHAR ||
                        prev.type == TokenType::RETURN) {
                        needSpaceBefore = true;
                    }
                }
                // 左括号前需要空格的情况
                else if (current.type == TokenType::LPAREN) {
                    if (prev.type == TokenType::IF || prev.type == TokenType::WHILE ||
                        prev.type == TokenType::FOR) {
                        needSpaceBefore = true;
                    }
                }
                // break, continue前需要空格
                else if (current.type == TokenType::BREAK || current.type == TokenType::CONTINUE) {
                    if (prev.type == TokenType::RPAREN || prev.type == TokenType::INTEGER) {
                        needSpaceBefore = true;
                    }
                }
                
                if (needSpaceBefore) {
                    formatted << " ";
                }
            }
        }
        
        // 添加当前token
        formatted << current.value;
        
        // 检查是否需要在当前token后加空格
        if (i < tokens.size() - 1) {
            Token& next = tokens[i+1];
            if (next.type != TokenType::EOF_TOKEN && next.type != TokenType::NEWLINE) {
                bool needSpaceAfter = false;
                
                // Hash后面跟include/define需要空格
                if (current.type == TokenType::HASH && 
                    (next.type == TokenType::INCLUDE || next.type == TokenType::DEFINE)) {
                    needSpaceAfter = true;
                }
                // include后面跟<需要空格
                else if (current.type == TokenType::INCLUDE && next.type == TokenType::LANGLE) {
                    needSpaceAfter = true;
                }
                // define后面跟标识符需要空格
                else if (current.type == TokenType::DEFINE && next.type == TokenType::IDENTIFIER) {
                    needSpaceAfter = true;
                }
                // 在#define中，标识符后面跟数字需要空格
                else if (current.type == TokenType::IDENTIFIER && 
                         (next.type == TokenType::FLOAT || next.type == TokenType::INTEGER)) {
                    // 检查前面是否有#define
                    bool inDefine = false;
                    for (int j = i - 1; j >= 0 && j >= (int)i - 4; j--) {
                        if (tokens[j].type == TokenType::DEFINE) {
                            inDefine = true;
                            break;
                        }
                    }
                    if (inDefine) {
                        needSpaceAfter = true;
                    }
                }
                // 类型关键字后面跟标识符需要空格
                else if ((current.type == TokenType::INT || current.type == TokenType::VOID ||
                         current.type == TokenType::FLOAT_KW || current.type == TokenType::CHAR) &&
                         next.type == TokenType::IDENTIFIER) {
                    needSpaceAfter = true;
                }
                // return后面跟值需要空格
                else if (current.type == TokenType::RETURN && 
                         (next.type == TokenType::INTEGER || next.type == TokenType::IDENTIFIER ||
                          next.type == TokenType::FLOAT)) {
                    needSpaceAfter = true;
                }
                // 二元操作符后需要空格
                else if (current.type == TokenType::ASSIGN || current.type == TokenType::EQ ||
                         current.type == TokenType::NE || current.type == TokenType::LT ||
                         current.type == TokenType::LE || current.type == TokenType::GT ||
                         current.type == TokenType::GE || current.type == TokenType::PLUS ||
                         current.type == TokenType::MINUS || current.type == TokenType::MULTIPLY ||
                         current.type == TokenType::DIVIDE || current.type == TokenType::AND ||
                         current.type == TokenType::OR ||
                         (current.type == TokenType::RANGLE && next.type != TokenType::SEMICOLON && 
                          i > 0 && tokens[i-1].type != TokenType::IDENTIFIER)) {
                    // 但是如果后面是右括号，不需要空格
                    if (next.type != TokenType::RPAREN) {
                        needSpaceAfter = true;
                    }
                }
                // 逗号后需要空格
                else if (current.type == TokenType::COMMA) {
                    needSpaceAfter = true;
                }
                // for循环中分号后需要空格，但不换行
                else if (current.type == TokenType::SEMICOLON && forDepth == 2) {
                    needSpaceAfter = true;
                }
                // 右括号和左大括号之间需要空格
                else if (current.type == TokenType::RPAREN && next.type == TokenType::LBRACE) {
                    needSpaceAfter = true;
                }
                
                if (needSpaceAfter) {
                    formatted << " ";
                }
            }
        }
        
        // 处理左大括号的缩进增加
        if (current.type == TokenType::LBRACE) {
            indentLevel++;
        }
        
        // 检查是否需要在当前token后换行
        if (i < tokens.size() - 1) {
            Token& next = tokens[i+1];
            bool shouldNewline = false;
            
            // 普通分号后需要换行，但for循环中的分号不换行
            if (current.type == TokenType::SEMICOLON && forDepth != 2) {
                shouldNewline = true;
            } 
            // 大括号后需要换行
            else if (current.type == TokenType::LBRACE || current.type == TokenType::RBRACE) {
                shouldNewline = true;
            } 
        // #include <xxx> 后需要换行
            else if (current.type == TokenType::RANGLE && i > 0) {
                // 检查前面是否有#include，扩大搜索范围
                for (int j = i - 1; j >= 0 && j >= (int)i - 6; j--) {
                    if (tokens[j].type == TokenType::INCLUDE) {
                        shouldNewline = true;
                        break;
                    }
                }
            }
            // #define后面的值（数字）后需要换行
            else if ((current.type == TokenType::FLOAT || current.type == TokenType::INTEGER) && i > 1) {
                // 检查前面是否有#define
                for (int j = i - 1; j >= 0 && j >= (int)i - 4; j--) {
                    if (tokens[j].type == TokenType::DEFINE) {
                        shouldNewline = true;
                        break;
                    }
                }
            }    
            
            if (shouldNewline && next.type != TokenType::EOF_TOKEN && 
                !(current.type == TokenType::RBRACE && next.type == TokenType::ELSE)) {
                formatted << "\n";
                atLineStart = true;
            }
        }
    }
    
    return formatted.str();
}
