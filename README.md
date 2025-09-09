# C++ 代码分析器

一个用C++实现的代码词法分析和语法分析程序，可以读取.txt文件中的代码并进行分析。

## 功能特性

- 📝 读取.txt文件中的代码
- 🔍 词法分析：将代码分解为tokens
- 🌳 语法分析：构建抽象语法树
- ❌ 错误检测：精确定位词法和语法错误
- 📍 错误报告：显示错误的行号和列号

## 项目结构

```
code_analyzer/
├── include/
│   ├── TokenTypes.h     # Token类型定义
│   ├── Lexer.h         # 词法分析器头文件
│   ├── Parser.h        # 语法分析器头文件
│   └── ErrorHandler.h  # 错误处理器头文件
├── src/
│   ├── TokenTypes.cpp  # Token类型实现
│   ├── Lexer.cpp       # 词法分析器实现
│   ├── Parser.cpp      # 语法分析器实现
│   ├── ErrorHandler.cpp# 错误处理器实现
│   └── main.cpp        # 主程序入口
├── test/               # 测试文件
│   ├── test_correct.txt
│   ├── test_lexical_error.txt
│   └── test_syntax_error.txt
├── Makefile           # 编译配置
└── README.md
```

## 编译和使用

### 编译
```bash
make
```

### 使用方法
```bash
./code_analyzer <code_file.txt>
```

## 支持的语法

目前支持简化的类C语言语法，包括：
- 变量声明 (int, float, char)
- 赋值语句
- 条件语句 (if-else)
- 循环语句 (while, for)
- 函数定义
- 表达式计算
- 算术运算符 (+, -, *, /, %)
- 比较运算符 (==, !=, <, <=, >, >=)
- 逻辑运算符 (&&, ||, !)

## 示例

### 正确的代码示例
```c
int main() {
    int x = 10;
    float y = 3.14;
    if (x > 5) {
        return 1;
    }
    return 0;
}
```

### 词法错误示例
```c
int x = 123.45.67;  // 错误：无效的数字格式
char c = 'unclosed;  // 错误：未闭合的字符串
```

### 语法错误示例
```c
int x =;  // 错误：缺少表达式
if (x > 5 {  // 错误：缺少右括号
```
