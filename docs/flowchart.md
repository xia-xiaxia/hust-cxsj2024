本流程图描述 Code Analyzer 的主要执行流程（词法分析 → 语法分析 → 格式化 / 错误报告）。

文件：`flowchart.svg` 为可视化流程图。

流程概要：
- Read file -> Lexer.tokenize
- 若词法错误，ErrorHandler 收集并报告（终止格式化）；否则进入 Parser.parse
- Parser.parse 构建 AST（并收集语法错误）；若存在语法错误，ErrorHandler 报告；否则 CodeFormatter.format 生成格式化代码

该图采用矩形节点和箭头表示模块间的数据流，已保存为 `flowchart.svg`。

本仓库同时提供将 SVG 渲染为 PNG 的本地脚本与 CI 工作流：

本地（PowerShell）:
	- 先安装 Python 依赖：
			python -m pip install -r tools/requirements.txt
	- 运行脚本（示例）：
			powershell.exe -ExecutionPolicy Bypass -File .\tools\svg_to_png.ps1 -Input "docs/flowchart.svg" -Output "docs/flowchart.png" -Width 1200 -Height 1600

GitHub Actions (CI):
	- 已添加工作流 `.github/workflows/svg-to-png.yml`，在 push 到 `docs/*.svg` 时会自动渲染 PNG，并将生成的 PNG 上传为 artifact（docs/png/*.png）。

备注：脚本优先尝试使用系统的 `rsvg-convert`（librsvg），若不可用则使用 Python 的 `cairosvg`（推荐）。