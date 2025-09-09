#include "../include/TokenTypes.h"
#include "../include/Lexer.h"
#include "../include/Parser.h"
#include "../include/ErrorHandler.h"
#include "../include/CodeFormatter.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>
#include <iomanip>

/**
 * 代码分析器主类
 * 整合词法分析器、语法分析器和错误处理器
 */
class CodeAnalyzer {
private:
    std::string sourceCode;
    std::unique_ptr<Lexer> lexer;
    std::unique_ptr<Parser> parser;
    std::unique_ptr<ErrorHandler> errorHandler;
    std::vector<Token> tokens;
    std::unique_ptr<ProgramNode> ast;
    
public:
    CodeAnalyzer() {
        errorHandler = std::make_unique<ErrorHandler>();
    }
    
    /**
     * 从文件读取源代码
     */
    bool loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file '" << filename << "'" << std::endl;
            return false;
        }
        
        std::ostringstream buffer;
        buffer << file.rdbuf();
        sourceCode = buffer.str();
        file.close();
        
        errorHandler->setSourceCode(sourceCode);
        
        std::cout << "Source code loaded from: " << filename << std::endl;
        std::cout << "File size: " << sourceCode.length() << " characters" << std::endl;
        
        return true;
    }
    
    /**
     * 设置源代码（用于直接输入代码）
     */
    void setSourceCode(const std::string& code) {
        sourceCode = code;
        errorHandler->setSourceCode(sourceCode);
    }
    
    /**
     * 执行词法分析
     */
    bool performLexicalAnalysis() {
        std::cout << "\n=== Lexical Analysis ===" << std::endl;
        
        lexer = std::make_unique<Lexer>(sourceCode);
        tokens = lexer->tokenize();
        
        // 收集词法错误
        if (lexer->hasErrors()) {
            errorHandler->addLexicalErrors(lexer->getErrors());
            std::cout << "Lexical analysis completed with errors." << std::endl;
            return false;
        } else {
            std::cout << "Lexical analysis completed successfully." << std::endl;
            std::cout << "Generated " << tokens.size() << " tokens." << std::endl;
            return true;
        }
    }
    
    /**
     * 执行语法分析
     */
    bool performSyntaxAnalysis() {
        if (tokens.empty()) {
            std::cout << "No tokens available for syntax analysis." << std::endl;
            return false;
        }
        
        std::cout << "\n=== Syntax Analysis ===" << std::endl;
        
        parser = std::make_unique<Parser>(tokens);
        ast = parser->parse();
        
        // 收集语法错误
        if (parser->hasErrors()) {
            errorHandler->addSyntaxErrors(parser->getErrors());
            std::cout << "Syntax analysis completed with errors." << std::endl;
            return false;
        } else {
            std::cout << "Syntax analysis completed successfully." << std::endl;
            std::cout << "Abstract Syntax Tree (AST) generated." << std::endl;
            return true;
        }
    }
    
    /**
     * 显示token列表
     */
    void showTokens() const {
        if (tokens.empty()) {
            std::cout << "No tokens to display." << std::endl;
            return;
        }
        
        std::cout << "\n=== Token List ===" << std::endl;
        for (size_t i = 0; i < tokens.size(); ++i) {
            const Token& token = tokens[i];
            if (token.type != TokenType::NEWLINE && token.type != TokenType::WHITESPACE) {
                std::cout << std::setw(3) << i << ": " << token << std::endl;
            }
        }
    }
    
    /**
     * 显示抽象语法树
     */
    void showAST() const {
        if (!ast) {
            std::cout << "No AST to display." << std::endl;
            return;
        }
        
        std::cout << "\n=== Abstract Syntax Tree ===" << std::endl;
        ast->print();
        
        // 递归打印所有语句
        for (size_t i = 0; i < ast->statements.size(); ++i) {
            std::cout << "Statement " << i + 1 << ":" << std::endl;
            ast->statements[i]->print(1);
        }
    }
    
    /**
     * 显示错误报告
     */
    void showErrorReport() const {
        std::cout << "\n";
        errorHandler->printDetailedReport();
    }
    
    /**
     * 执行完整的代码分析
     */
    void analyze(bool showDetails = false) {
        std::cout << "Starting code analysis..." << std::endl;
        
        // 词法分析
        bool lexicalSuccess = performLexicalAnalysis();
        
        if (showDetails && lexicalSuccess) {
            showTokens();
        }
        
        // 如果词法分析成功，继续语法分析
        bool syntaxSuccess = false;
        if (lexicalSuccess) {
            syntaxSuccess = performSyntaxAnalysis();
            
            if (showDetails && syntaxSuccess) {
                showAST();
            }
        }
        
        // 显示最终错误报告
        showErrorReport();
        
        // 分析完成总结
        std::cout << "\n=== Analysis Complete ===" << std::endl;
        if (lexicalSuccess && syntaxSuccess) {
            std::cout << "✓ Code analysis completed successfully!" << std::endl;
        } else {
            std::cout << "✗ Code analysis found errors." << std::endl;
        }
    }
    
    /**
     * 获取分析结果
     */
    bool hasErrors() const {
        return errorHandler->hasErrors();
    }
    
    size_t getErrorCount() const {
        return errorHandler->getErrorCount();
    }
    
    /**
     * 获取token列表（用于代码格式化）
     */
    const std::vector<Token>& getTokens() const {
        return tokens;
    }
};

/**
 * 显示程序使用帮助
 */
void showUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options] <code_file.txt>" << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  -h, --help     Show this help message" << std::endl;
    std::cout << "  -v, --verbose  Show detailed analysis (tokens and AST)" << std::endl;
    std::cout << "  -t, --tokens   Show only token analysis" << std::endl;
    std::cout << "  -s, --syntax   Show only syntax analysis" << std::endl;
    std::cout << "  -f, --format   Format and output the code (if syntactically correct)" << std::endl;
    std::cout << "\nExample:" << std::endl;
    std::cout << "  " << programName << " test.txt" << std::endl;
    std::cout << "  " << programName << " -v test.txt" << std::endl;
    std::cout << "  " << programName << " -f test.txt" << std::endl;
}

/**
 * 主函数
 */
int main(int argc, char* argv[]) {
    std::cout << "=== C++ Code Analyzer ===" << std::endl;
    std::cout << "Version 1.0 - Lexical and Syntax Analysis Tool" << std::endl;
    
    if (argc < 2) {
        showUsage(argv[0]);
        return 1;
    }
    
    bool showDetails = false;
    bool tokensOnly = false;
    bool syntaxOnly = false;
    bool formatOnly = false;
    std::string filename;
    
    // 解析命令行参数
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            showUsage(argv[0]);
            return 0;
        } else if (arg == "-v" || arg == "--verbose") {
            showDetails = true;
        } else if (arg == "-t" || arg == "--tokens") {
            tokensOnly = true;
        } else if (arg == "-s" || arg == "--syntax") {
            syntaxOnly = true;
        } else if (arg == "-f" || arg == "--format") {
            formatOnly = true;
        } else if (arg[0] != '-') {
            filename = arg;
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            showUsage(argv[0]);
            return 1;
        }
    }
    
    if (filename.empty()) {
        std::cerr << "Error: No input file specified." << std::endl;
        showUsage(argv[0]);
        return 1;
    }
    
    // 创建代码分析器
    CodeAnalyzer analyzer;
    
    // 加载源代码文件
    if (!analyzer.loadFromFile(filename)) {
        return 1;
    }
    
    try {
        if (tokensOnly) {
            // 只执行词法分析
            analyzer.performLexicalAnalysis();
            analyzer.showTokens();
            analyzer.showErrorReport();
        } else if (syntaxOnly) {
            // 执行词法和语法分析，但只显示语法相关结果
            analyzer.performLexicalAnalysis();
            analyzer.performSyntaxAnalysis();
            analyzer.showAST();
            analyzer.showErrorReport();
        } else if (formatOnly) {
            // 格式化代码功能
            analyzer.performLexicalAnalysis();
            analyzer.performSyntaxAnalysis();
            
            if (analyzer.hasErrors()) {
                std::cout << "\n=== Code Formatting Failed ===" << std::endl;
                std::cout << "Cannot format code due to errors. Please fix the following issues first:\n" << std::endl;
                analyzer.showErrorReport();
                return 1;
            } else {
                std::cout << "\n=== Formatted Code ===" << std::endl;
                CodeFormatter formatter;
                std::string formattedCode = formatter.format(analyzer.getTokens());
                std::cout << formattedCode << std::endl;
                std::cout << "\n=== Formatting Complete ===" << std::endl;
            }
        } else {
            // 执行完整分析
            analyzer.analyze(showDetails);
        }
        
        // 返回适当的退出代码
        return analyzer.hasErrors() ? 1 : 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error during analysis: " << e.what() << std::endl;
        return 1;
    }
}
