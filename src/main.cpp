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
        
        // 使用中文格式打印所有语句
        for (size_t i = 0; i < ast->statements.size(); ++i) {
            if (ast->statements[i]) {
                ast->statements[i]->printChinese(0);
            }
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
            
            if (syntaxSuccess) {
                showAST(); // 总是显示AST
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
    std::cout << "  -h, --help       Show this help message" << std::endl;
    std::cout << "  -i, --interactive Start interactive mode" << std::endl;
    std::cout << "  -v, --verbose    Show detailed analysis (tokens and AST)" << std::endl;
    std::cout << "  -t, --tokens     Show only token analysis" << std::endl;
    std::cout << "  -s, --syntax     Show only syntax analysis" << std::endl;
    std::cout << "  -f, --format     Format and output the code (if syntactically correct)" << std::endl;
    std::cout << "  -o, --output     Output formatted code to 'out' file" << std::endl;
    std::cout << "\nExample:" << std::endl;
    std::cout << "  " << programName << " -i                # Interactive mode" << std::endl;
    std::cout << "  " << programName << " test.txt          # Basic analysis" << std::endl;
    std::cout << "  " << programName << " -v test.txt       # Detailed analysis" << std::endl;
    std::cout << "  " << programName << " -f test.txt       # Format code" << std::endl;
    std::cout << "  " << programName << " -o test.txt       # Output to file" << std::endl;
}

/**
 * 显示交互式菜单
 */
void showMenu() {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "           🔍 C++ 代码分析器 - 交互式界面" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << std::endl;
    std::cout << "📁 请选择分析功能：" << std::endl;
    std::cout << std::endl;
    std::cout << "  1️⃣  完整代码分析          - 词法+语法分析+AST输出" << std::endl;
    std::cout << "  2️⃣  详细分析模式          - 显示Token列表+完整AST结构" << std::endl;
    std::cout << "  3️⃣  仅词法分析            - 只显示Token分词结果" << std::endl;
    std::cout << "  4️⃣  仅语法分析            - 只显示语法结构(AST)" << std::endl;
    std::cout << "  5️⃣  代码格式化            - 自动格式化并美化代码" << std::endl;
    std::cout << "  6️⃣  错误检测模式          - 专注于语法错误检测" << std::endl;
    std::cout << "  7️⃣  帮助信息              - 显示使用说明" << std::endl;
    std::cout << "  8️⃣  输出正确代码到文件    - 保存格式化代码到out文件" << std::endl;
    std::cout << "  0️⃣  退出程序              - Exit" << std::endl;
    std::cout << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    std::cout << "💡 提示: 请先确保您的代码文件放在当前目录下" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

/**
 * 显示功能说明
 */
void showFeatureDescription() {
    std::cout << "\n📖 功能详细说明：" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    std::cout << std::endl;
    std::cout << "🔹 词法分析 (Lexical Analysis):" << std::endl;
    std::cout << "   将源代码分解为Token(词法单元)，如关键字、标识符、运算符等" << std::endl;
    std::cout << "   检测无法识别的字符和词法错误" << std::endl;
    std::cout << std::endl;
    std::cout << "🔹 语法分析 (Syntax Analysis):" << std::endl;
    std::cout << "   基于Token序列构建抽象语法树(AST)" << std::endl;
    std::cout << "   检测语法错误，如括号不匹配、类型错误等" << std::endl;
    std::cout << std::endl;
    std::cout << "🔹 中文AST输出:" << std::endl;
    std::cout << "   以中文形式展示程序的语法结构" << std::endl;
    std::cout << "   支持函数、变量、表达式、控制语句等的结构化显示" << std::endl;
    std::cout << std::endl;
    std::cout << "🔹 代码格式化:" << std::endl;
    std::cout << "   自动调整代码缩进、空格、换行" << std::endl;
    std::cout << "   提高代码可读性和一致性" << std::endl;
    std::cout << std::endl;
    std::cout << "🔹 错误检测:" << std::endl;
    std::cout << "   提供详细的错误位置和错误描述" << std::endl;
    std::cout << "   支持错误恢复，继续分析后续代码" << std::endl;
    std::cout << std::endl;
}

/**
 * 获取用户输入的文件名
 */
std::string getFilename() {
    std::string filename;
    std::cout << "\n📂 请输入代码文件名 (例: 111.txt, test.cpp): ";
    std::getline(std::cin, filename);
    
    // 去掉首尾空格
    size_t start = filename.find_first_not_of(" \t");
    if (start == std::string::npos) return "";
    
    size_t end = filename.find_last_not_of(" \t");
    filename = filename.substr(start, end - start + 1);
    
    return filename;
}

/**
 * 暂停等待用户输入
 */
void pauseForUser() {
    std::cout << "\n按 Enter 键继续...";
    std::cin.ignore();
    std::string dummy;
    std::getline(std::cin, dummy);
}

/**
 * 交互式模式主循环
 */
void interactiveMode() {
    int choice;
    std::string filename;
    
    while (true) {
        showMenu();
        std::cout << "请选择功能 (0-8): ";
        
        // 读取用户选择
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "\n❌ 输入无效，请输入数字 0-8" << std::endl;
            pauseForUser();
            continue;
        }
        std::cin.ignore(); // 清除换行符
        
        switch (choice) {
            case 0: {
                std::cout << "\n👋 感谢使用 C++ 代码分析器！再见！" << std::endl;
                return;
            }
            
            case 1: { // 完整代码分析
                filename = getFilename();
                if (filename.empty()) {
                    std::cout << "❌ 文件名不能为空" << std::endl;
                    break;
                }
                
                std::cout << "\n🔍 执行完整代码分析..." << std::endl;
                CodeAnalyzer analyzer;
                if (analyzer.loadFromFile(filename)) {
                    analyzer.analyze(false); // 标准模式
                }
                pauseForUser();
                break;
            }
            
            case 2: { // 详细分析模式
                filename = getFilename();
                if (filename.empty()) {
                    std::cout << "❌ 文件名不能为空" << std::endl;
                    break;
                }
                
                std::cout << "\n🔍 执行详细分析模式..." << std::endl;
                CodeAnalyzer analyzer;
                if (analyzer.loadFromFile(filename)) {
                    analyzer.analyze(true); // 详细模式
                }
                pauseForUser();
                break;
            }
            
            case 3: { // 仅词法分析
                filename = getFilename();
                if (filename.empty()) {
                    std::cout << "❌ 文件名不能为空" << std::endl;
                    break;
                }
                
                std::cout << "\n🔤 执行词法分析..." << std::endl;
                CodeAnalyzer analyzer;
                if (analyzer.loadFromFile(filename)) {
                    analyzer.performLexicalAnalysis();
                    analyzer.showTokens();
                    analyzer.showErrorReport();
                }
                pauseForUser();
                break;
            }
            
            case 4: { // 仅语法分析
                filename = getFilename();
                if (filename.empty()) {
                    std::cout << "❌ 文件名不能为空" << std::endl;
                    break;
                }
                
                std::cout << "\n🏗️  执行语法分析..." << std::endl;
                CodeAnalyzer analyzer;
                if (analyzer.loadFromFile(filename)) {
                    analyzer.performLexicalAnalysis();
                    analyzer.performSyntaxAnalysis();
                    analyzer.showAST();
                    analyzer.showErrorReport();
                }
                pauseForUser();
                break;
            }
            
            case 5: { // 代码格式化
                filename = getFilename();
                if (filename.empty()) {
                    std::cout << "❌ 文件名不能为空" << std::endl;
                    break;
                }
                
                std::cout << "\n✨ 执行代码格式化..." << std::endl;
                CodeAnalyzer analyzer;
                if (analyzer.loadFromFile(filename)) {
                    analyzer.performLexicalAnalysis();
                    analyzer.performSyntaxAnalysis();
                    
                    if (analyzer.hasErrors()) {
                        std::cout << "\n❌ 代码格式化失败" << std::endl;
                        std::cout << "由于存在语法错误，无法格式化代码。请先修复以下问题：\n" << std::endl;
                        analyzer.showErrorReport();
                    } else {
                        std::cout << "\n✅ 格式化成功！格式化后的代码：" << std::endl;
                        std::cout << std::string(50, '=') << std::endl;
                        CodeFormatter formatter;
                        std::string formattedCode = formatter.format(analyzer.getTokens());
                        std::cout << formattedCode << std::endl;
                        std::cout << std::string(50, '=') << std::endl;
                        std::cout << "格式化完成！" << std::endl;
                    }
                }
                pauseForUser();
                break;
            }
            
            case 6: { // 错误检测模式
                filename = getFilename();
                if (filename.empty()) {
                    std::cout << "❌ 文件名不能为空" << std::endl;
                    break;
                }
                
                std::cout << "\n🐛 执行错误检测..." << std::endl;
                CodeAnalyzer analyzer;
                if (analyzer.loadFromFile(filename)) {
                    analyzer.performLexicalAnalysis();
                    analyzer.performSyntaxAnalysis();
                    
                    if (!analyzer.hasErrors()) {
                        std::cout << "\n✅ 恭喜！代码语法正确，未发现错误！" << std::endl;
                    } else {
                        std::cout << "\n⚠️  发现语法错误，详细信息如下：" << std::endl;
                    }
                    analyzer.showErrorReport();
                }
                pauseForUser();
                break;
            }
            
            case 7: { // 帮助信息
                showFeatureDescription();
                pauseForUser();
                break;
            }
            
            case 8: { // 输出正确代码到文件
                filename = getFilename();
                if (filename.empty()) {
                    std::cout << "❌ 文件名不能为空" << std::endl;
                    break;
                }
                
                std::cout << "\n📄 输出正确代码到文件..." << std::endl;
                CodeAnalyzer analyzer;
                if (analyzer.loadFromFile(filename)) {
                    analyzer.performLexicalAnalysis();
                    analyzer.performSyntaxAnalysis();
                    
                    if (analyzer.hasErrors()) {
                        std::cout << "\n❌ 代码输出失败" << std::endl;
                        std::cout << "由于存在语法错误，无法输出代码。请先修复以下问题：\n" << std::endl;
                        analyzer.showErrorReport();
                    } else {
                        // 输出格式化代码到out文件
                        CodeFormatter formatter;
                        std::string formattedCode = formatter.format(analyzer.getTokens());
                        
                        std::string outputFilename = "out.txt";
                        std::ofstream outFile(outputFilename);
                        if (outFile.is_open()) {
                            outFile << formattedCode;
                            outFile.close();
                            
                            std::cout << "\n✅ 代码已成功输出到文件: " << outputFilename << std::endl;
                            std::cout << "文件大小: " << formattedCode.length() << " 字符" << std::endl;
                            std::cout << "您可以查看 '" << outputFilename << "' 文件来查看格式化后的代码。" << std::endl;
                        } else {
                            std::cout << "\n❌ 无法创建输出文件: " << outputFilename << std::endl;
                        }
                    }
                }
                pauseForUser();
                break;
            }
            
            default: {
                std::cout << "\n❌ 选择无效，请输入 0-8 之间的数字" << std::endl;
                pauseForUser();
                break;
            }
        }
    }
}

/**
 * 主函数
 */
int main(int argc, char* argv[]) {
    std::cout << "=== C++ Code Analyzer ===" << std::endl;
    std::cout << "Version 1.0 - Lexical and Syntax Analysis Tool" << std::endl;
    
    // 如果没有参数，启动交互模式
    if (argc < 2) {
        std::cout << "\n🚀 启动交互式界面..." << std::endl;
        interactiveMode();
        return 0;
    }
    
    bool showDetails = false;
    bool tokensOnly = false;
    bool syntaxOnly = false;
    bool formatOnly = false;
    bool outputOnly = false;
    bool interactiveFlag = false;
    std::string filename;
    
    // 解析命令行参数
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            showUsage(argv[0]);
            return 0;
        } else if (arg == "-i" || arg == "--interactive") {
            interactiveFlag = true;
        } else if (arg == "-v" || arg == "--verbose") {
            showDetails = true;
        } else if (arg == "-t" || arg == "--tokens") {
            tokensOnly = true;
        } else if (arg == "-s" || arg == "--syntax") {
            syntaxOnly = true;
        } else if (arg == "-f" || arg == "--format") {
            formatOnly = true;
        } else if (arg == "-o" || arg == "--output") {
            outputOnly = true;
        } else if (arg[0] != '-') {
            filename = arg;
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            showUsage(argv[0]);
            return 1;
        }
    }
    
    // 如果指定了交互模式参数，启动交互模式
    if (interactiveFlag) {
        std::cout << "\n🚀 启动交互式界面..." << std::endl;
        interactiveMode();
        return 0;
    }
    
    if (filename.empty()) {
        std::cerr << "Error: No input file specified." << std::endl;
        std::cout << "\n💡 提示: 使用 " << argv[0] << " -i 启动交互式界面" << std::endl;
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
            std::cout << "\n🔤 执行词法分析..." << std::endl;
            analyzer.performLexicalAnalysis();
            analyzer.showTokens();
            analyzer.showErrorReport();
        } else if (syntaxOnly) {
            // 执行词法和语法分析，但只显示语法相关结果
            std::cout << "\n🏗️  执行语法分析..." << std::endl;
            analyzer.performLexicalAnalysis();
            analyzer.performSyntaxAnalysis();
            analyzer.showAST();
            analyzer.showErrorReport();
        } else if (formatOnly) {
            // 格式化代码功能
            std::cout << "\n✨ 执行代码格式化..." << std::endl;
            analyzer.performLexicalAnalysis();
            analyzer.performSyntaxAnalysis();
            
            if (analyzer.hasErrors()) {
                std::cout << "\n❌ 代码格式化失败" << std::endl;
                std::cout << "由于存在语法错误，无法格式化代码。请先修复以下问题：\n" << std::endl;
                analyzer.showErrorReport();
                return 1;
            } else {
                std::cout << "\n✅ 格式化成功！格式化后的代码：" << std::endl;
                std::cout << std::string(50, '=') << std::endl;
                CodeFormatter formatter;
                std::string formattedCode = formatter.format(analyzer.getTokens());
                std::cout << formattedCode << std::endl;
                std::cout << std::string(50, '=') << std::endl;
                std::cout << "格式化完成！" << std::endl;
            }
        } else if (outputOnly) {
            // 输出代码到文件功能
            std::cout << "\n📄 输出代码到文件..." << std::endl;
            analyzer.performLexicalAnalysis();
            analyzer.performSyntaxAnalysis();
            
            if (analyzer.hasErrors()) {
                std::cout << "\n❌ 代码输出失败" << std::endl;
                std::cout << "由于存在语法错误，无法输出代码。请先修复以下问题：\n" << std::endl;
                analyzer.showErrorReport();
                return 1;
            } else {
                // 输出格式化代码到out文件
                CodeFormatter formatter;
                std::string formattedCode = formatter.format(analyzer.getTokens());
                
                std::string outputFilename = "out.txt";
                std::ofstream outFile(outputFilename);
                if (outFile.is_open()) {
                    outFile << formattedCode;
                    outFile.close();
                    
                    std::cout << "\n✅ 代码已成功输出到文件: " << outputFilename << std::endl;
                    std::cout << "文件大小: " << formattedCode.length() << " 字符" << std::endl;
                    std::cout << "您可以查看 '" << outputFilename << "' 文件来查看格式化后的代码。" << std::endl;
                } else {
                    std::cout << "\n❌ 无法创建输出文件: " << outputFilename << std::endl;
                    return 1;
                }
            }
        } else {
            // 执行完整分析
            std::cout << "\n🔍 执行" << (showDetails ? "详细" : "完整") << "代码分析..." << std::endl;
            analyzer.analyze(showDetails);
        }
        
        // 返回适当的退出代码
        return analyzer.hasErrors() ? 1 : 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error during analysis: " << e.what() << std::endl;
        return 1;
    }
}
