# Lamina

Lamina-是Lamina Math DSL 的重新实现， 语法标准与 新LSR 一致

## 项目结构

```
lamina/
├── common/           # 通用模块，包含文件运行和REPL环境
├── compiler/         # 编译器，包含词法分析、语法分析和代码生成
├── example/          # 示例代码
├── include/          # 公共头文件
├── runtime/          # 运行时环境，包含虚拟机实现
├── main.cpp          # 主入口文件
└── README.md         # 本文档
```

## 安装

### 从源码构建

1. **克隆仓库**

```bash
git clone https://github.com/lamina-dev/lamina-.git
cd lamina
```

2. **编译项目**

```bash
# 使用CMake构建
mkdir build
cd build
cmake ..
cmake --build .
```


## 使用

```bash
lamina hello.lm
```

### 使用REPL环境

启动交互式REPL环境：

```bash
./lamina
```
在 Windows 上，你可能需要使用 `.\lamina.exe` 来启动 REPL。

在REPL中输入代码并执行：

```lamina
> let x = 5
> let y = 10
> x + y
15
```