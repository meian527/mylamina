# Lamina

Lamina-是Lamina Math DSL 的重新实现， 语法标准与 新LSR 一致



### 从源码构建

1. **克隆仓库**

```bash
git clone https://github.com/lamina-dev/mylamina.git
cd mylamina
```

2. **编译项目**

```bash
# 使用CMake构建
cmake -B build
cmake --build build --config Release --parallel
```


## 使用

```bash
./build/bin/lm xxx.lm
```

### 使用REPL环境

启动交互式REPL环境：

```bash
./build/bin/lm
```
在 Windows 上，你可能需要使用 `.\lm.exe` 来启动 REPL。

在REPL中输入代码并执行：

```lamina
> let x = 5
> let y = 10
> x + y
15
```