# Makefile for C++ Code Analyzer
# 编译配置

# 编译器设置
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
DEBUG_FLAGS = -std=c++17 -Wall -Wextra -g -DDEBUG

# 目录设置
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
TEST_DIR = test

# 目标可执行文件
TARGET = code_analyzer
DEBUG_TARGET = code_analyzer_debug

# 源文件
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEBUG_OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/debug_%.o)

# 默认目标
.PHONY: all clean debug test help install

all: $(TARGET)

# 编译发布版本
$(TARGET): $(BUILD_DIR) $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)
	@echo "Build complete: $(TARGET)"

# 编译调试版本
debug: $(DEBUG_TARGET)

$(DEBUG_TARGET): $(BUILD_DIR) $(DEBUG_OBJECTS)
	$(CXX) $(DEBUG_OBJECTS) -o $(DEBUG_TARGET)
	@echo "Debug build complete: $(DEBUG_TARGET)"

# 创建构建目录
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# 编译目标文件 (发布版本)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# 编译目标文件 (调试版本)
$(BUILD_DIR)/debug_%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(DEBUG_FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# 运行测试
test: $(TARGET)
	@echo "Running tests..."
	@if [ -f $(TEST_DIR)/test_correct.txt ]; then \
		echo "Testing correct code:"; \
		./$(TARGET) $(TEST_DIR)/test_correct.txt; \
	fi
	@if [ -f $(TEST_DIR)/test_lexical_error.txt ]; then \
		echo "\nTesting lexical errors:"; \
		./$(TARGET) $(TEST_DIR)/test_lexical_error.txt || true; \
	fi
	@if [ -f $(TEST_DIR)/test_syntax_error.txt ]; then \
		echo "\nTesting syntax errors:"; \
		./$(TARGET) $(TEST_DIR)/test_syntax_error.txt || true; \
	fi

# 详细测试
test-verbose: $(TARGET)
	@echo "Running verbose tests..."
	@if [ -f $(TEST_DIR)/test_correct.txt ]; then \
		echo "Testing correct code (verbose):"; \
		./$(TARGET) -v $(TEST_DIR)/test_correct.txt; \
	fi

# 安装到系统目录
install: $(TARGET)
	@echo "Installing $(TARGET) to /usr/local/bin/"
	@sudo cp $(TARGET) /usr/local/bin/
	@echo "Installation complete"

# 清理构建文件
clean:
	@rm -rf $(BUILD_DIR)
	@rm -f $(TARGET) $(DEBUG_TARGET)
	@echo "Clean complete"

# 显示帮助信息
help:
	@echo "Available targets:"
	@echo "  all          - Build release version (default)"
	@echo "  debug        - Build debug version"
	@echo "  test         - Run tests with sample files"
	@echo "  test-verbose - Run tests with verbose output"
	@echo "  clean        - Remove build files and executables"
	@echo "  install      - Install to /usr/local/bin"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "Usage examples:"
	@echo "  make                    # Build release version"
	@echo "  make debug             # Build debug version"
	@echo "  make test              # Run tests"
	@echo "  ./code_analyzer file.txt       # Analyze file.txt"
	@echo "  ./code_analyzer -v file.txt    # Verbose analysis"

# 依赖关系
$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cpp $(INCLUDE_DIR)/TokenTypes.h $(INCLUDE_DIR)/Lexer.h $(INCLUDE_DIR)/Parser.h $(INCLUDE_DIR)/ErrorHandler.h
$(BUILD_DIR)/TokenTypes.o: $(SRC_DIR)/TokenTypes.cpp $(INCLUDE_DIR)/TokenTypes.h
$(BUILD_DIR)/Lexer.o: $(SRC_DIR)/Lexer.cpp $(INCLUDE_DIR)/Lexer.h $(INCLUDE_DIR)/TokenTypes.h
$(BUILD_DIR)/Parser.o: $(SRC_DIR)/Parser.cpp $(INCLUDE_DIR)/Parser.h $(INCLUDE_DIR)/TokenTypes.h $(INCLUDE_DIR)/Lexer.h
$(BUILD_DIR)/ErrorHandler.o: $(SRC_DIR)/ErrorHandler.cpp $(INCLUDE_DIR)/ErrorHandler.h $(INCLUDE_DIR)/TokenTypes.h $(INCLUDE_DIR)/Lexer.h $(INCLUDE_DIR)/Parser.h
