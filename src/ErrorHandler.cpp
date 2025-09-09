#include "../include/ErrorHandler.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

// ErrorInfo结构实现
std::string ErrorInfo::toString() const {
    std::ostringstream oss;
    oss << "[" << (type == ErrorType::LEXICAL_ERROR ? "LEXICAL" : "SYNTAX") 
        << "] Line " << line << ", Column " << column << ": " << message;
    return oss.str();
}

// ErrorHandler类实现
ErrorHandler::ErrorHandler(const std::string& sourceCode) {
    setSourceCode(sourceCode);
}

void ErrorHandler::setSourceCode(const std::string& sourceCode) {
    sourceLines.clear();
    std::istringstream iss(sourceCode);
    std::string line;
    while (std::getline(iss, line)) {
        sourceLines.push_back(line);
    }
}

void ErrorHandler::addLexicalError(const LexicalError& lexError) {
    std::string context = extractSourceContext(lexError.line, lexError.column);
    errors.emplace_back(ErrorType::LEXICAL_ERROR, lexError.message, 
                       lexError.line, lexError.column, context);
}

void ErrorHandler::addSyntaxError(const SyntaxError& syntaxError) {
    std::string context = extractSourceContext(syntaxError.line, syntaxError.column);
    errors.emplace_back(ErrorType::SYNTAX_ERROR, syntaxError.message, 
                       syntaxError.line, syntaxError.column, context);
}

void ErrorHandler::addLexicalErrors(const std::vector<LexicalError>& lexErrors) {
    for (const auto& error : lexErrors) {
        addLexicalError(error);
    }
}

void ErrorHandler::addSyntaxErrors(const std::vector<SyntaxError>& syntaxErrors) {
    for (const auto& error : syntaxErrors) {
        addSyntaxError(error);
    }
}

bool ErrorHandler::hasErrors() const {
    return !errors.empty();
}

bool ErrorHandler::hasLexicalErrors() const {
    return std::any_of(errors.begin(), errors.end(), 
                      [](const ErrorInfo& error) { 
                          return error.type == ErrorType::LEXICAL_ERROR; 
                      });
}

bool ErrorHandler::hasSyntaxErrors() const {
    return std::any_of(errors.begin(), errors.end(), 
                      [](const ErrorInfo& error) { 
                          return error.type == ErrorType::SYNTAX_ERROR; 
                      });
}

size_t ErrorHandler::getErrorCount() const {
    return errors.size();
}

size_t ErrorHandler::getLexicalErrorCount() const {
    return std::count_if(errors.begin(), errors.end(), 
                        [](const ErrorInfo& error) { 
                            return error.type == ErrorType::LEXICAL_ERROR; 
                        });
}

size_t ErrorHandler::getSyntaxErrorCount() const {
    return std::count_if(errors.begin(), errors.end(), 
                        [](const ErrorInfo& error) { 
                            return error.type == ErrorType::SYNTAX_ERROR; 
                        });
}

const std::vector<ErrorInfo>& ErrorHandler::getErrors() const {
    return errors;
}

std::vector<ErrorInfo> ErrorHandler::getLexicalErrors() const {
    std::vector<ErrorInfo> lexErrors;
    std::copy_if(errors.begin(), errors.end(), std::back_inserter(lexErrors),
                [](const ErrorInfo& error) { 
                    return error.type == ErrorType::LEXICAL_ERROR; 
                });
    return lexErrors;
}

std::vector<ErrorInfo> ErrorHandler::getSyntaxErrors() const {
    std::vector<ErrorInfo> syntaxErrors;
    std::copy_if(errors.begin(), errors.end(), std::back_inserter(syntaxErrors),
                [](const ErrorInfo& error) { 
                    return error.type == ErrorType::SYNTAX_ERROR; 
                });
    return syntaxErrors;
}

void ErrorHandler::printErrors() const {
    if (errors.empty()) {
        std::cout << "No errors found." << std::endl;
        return;
    }
    
    for (const auto& error : errors) {
        std::cout << error.toString() << std::endl;
        if (!error.context.empty()) {
            showErrorContext(error);
        }
    }
}

void ErrorHandler::printSummary() const {
    size_t lexicalCount = getLexicalErrorCount();
    size_t syntaxCount = getSyntaxErrorCount();
    size_t totalCount = getErrorCount();
    
    std::cout << "\n=== Error Summary ===" << std::endl;
    std::cout << "Total errors: " << totalCount << std::endl;
    std::cout << "Lexical errors: " << lexicalCount << std::endl;
    std::cout << "Syntax errors: " << syntaxCount << std::endl;
    
    if (totalCount == 0) {
        std::cout << "✓ Code analysis completed successfully!" << std::endl;
    } else {
        std::cout << "✗ Code analysis found " << totalCount << " error(s)." << std::endl;
    }
}

void ErrorHandler::printDetailedReport() const {
    std::cout << "=== Detailed Error Report ===" << std::endl;
    
    if (errors.empty()) {
        std::cout << "✓ No errors found. Code is syntactically correct!" << std::endl;
        return;
    }
    
    // 按类型分组显示错误
    auto lexErrors = getLexicalErrors();
    auto syntaxErrors = getSyntaxErrors();
    
    if (!lexErrors.empty()) {
        std::cout << "\n--- Lexical Errors (" << lexErrors.size() << ") ---" << std::endl;
        for (const auto& error : lexErrors) {
            std::cout << error.toString() << std::endl;
            showErrorContext(error);
            std::cout << std::endl;
        }
    }
    
    if (!syntaxErrors.empty()) {
        std::cout << "\n--- Syntax Errors (" << syntaxErrors.size() << ") ---" << std::endl;
        for (const auto& error : syntaxErrors) {
            std::cout << error.toString() << std::endl;
            showErrorContext(error);
            std::cout << std::endl;
        }
    }
    
    printSummary();
}

std::string ErrorHandler::generateErrorReport() const {
    std::ostringstream oss;
    oss << "=== Code Analysis Report ===\n";
    
    if (errors.empty()) {
        oss << "✓ No errors found. Code is syntactically correct!\n";
        return oss.str();
    }
    
    for (const auto& error : errors) {
        oss << error.toString() << "\n";
        if (!error.context.empty()) {
            oss << "Context: " << error.context << "\n";
        }
    }
    
    oss << generateSummary();
    return oss.str();
}

std::string ErrorHandler::generateSummary() const {
    std::ostringstream oss;
    size_t lexicalCount = getLexicalErrorCount();
    size_t syntaxCount = getSyntaxErrorCount();
    size_t totalCount = getErrorCount();
    
    oss << "\n=== Summary ===\n";
    oss << "Total errors: " << totalCount << "\n";
    oss << "Lexical errors: " << lexicalCount << "\n";
    oss << "Syntax errors: " << syntaxCount << "\n";
    
    if (totalCount == 0) {
        oss << "✓ Analysis completed successfully!\n";
    } else {
        oss << "✗ Analysis found " << totalCount << " error(s).\n";
    }
    
    return oss.str();
}

void ErrorHandler::clearErrors() {
    errors.clear();
}

std::string ErrorHandler::getErrorTypeString(ErrorType type) const {
    switch (type) {
        case ErrorType::LEXICAL_ERROR:
            return "LEXICAL";
        case ErrorType::SYNTAX_ERROR:
            return "SYNTAX";
        default:
            return "UNKNOWN";
    }
}

std::string ErrorHandler::extractSourceContext(int line, int column) const {
    if (line <= 0 || line > static_cast<int>(sourceLines.size())) {
        return "";
    }
    
    return sourceLines[line - 1];  // 行号从1开始，数组从0开始
}

void ErrorHandler::showErrorContext(const ErrorInfo& error) const {
    if (error.context.empty()) {
        return;
    }
    
    std::cout << "  | " << error.context << std::endl;
    
    // 显示错误位置指示器
    std::cout << "  | ";
    for (int i = 1; i < error.column; ++i) {
        std::cout << " ";
    }
    std::cout << "^" << std::endl;
}

std::string ErrorHandler::formatErrorMessage(const std::string& message, int line, int column) {
    std::ostringstream oss;
    oss << "Line " << line << ", Column " << column << ": " << message;
    return oss.str();
}
