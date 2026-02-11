# SimpleLang Compiler

A compiler that translates SimpleLang code into assembly 
for an 8-bit CPU.

## What is SimpleLang?
SimpleLang is a basic high level language that supports:
- Variable declarations: int a;
- Assignments: a = 10;
- Arithmetic: c = a + b;
- Conditionals: if (a == b) { }

## How to use

### Compile SimpleLang to Assembly
```bash
cd compiler
g++ -o compiler simplelang_complete.cpp
./compiler ../programs/input.simple
```

### Run on 8-bit CPU
```bash
cd 8bit-computer
python3 asm/asm.py ../programs/input.asm > memory.list
make clean && make run
```

## Project Structure
```
simplelang-compiler/
├── compiler/
│   └── simplelang_complete.cpp
├── programs/
│   └── input.simple
├── output/
│   └── input.asm
└── 8bit-computer/
```
