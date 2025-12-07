# Hack Assembler  

An assembler for the Hack Assembly Language written in C++. The assembler follows the specification of the Hack Assembly
and Machine Languages as described in *The Elements of Computing Systems* and [Nand2Tetris](https://www.nand2tetris.org).  

## Usage  

Run the compiled binary and pass a path to the input assembly file as the first argument. Optionally a path to the output
file may be specified as the second argument.

```bash
$ Assembler <input path> [output path]
```

If successful, the program will output `input path -> output path` and write machine language instruction in the output file
as newline separated sequences of 16 "0" and "1" characters. If an error is encountered, the program will output a message 
like the following.

``` 
input.asm:21:8 Label has already been defined
  (LOOP)
   ~~~~
```
## Building  

This project uses the [Premake](https://premake.github.io/) build system which will be automatically built during the
setup process. Start by cloning the repository with `git clone --recursive https://github.com/TuiskuTynkkynen/HackAssembler.git`
and follow the instructions for your platform.

### Windows  

- Dependencies  
  - MSVC C++ toolset
- Run [Scripts/Setup-Windows.bat](Scripts/Setup-Windows.bat)  
- Open the generated Visual Studio Solution 

### Linux

- Dependencies  
  - make  
  - clang 
  - libuuid-devel  
- Run [Scripts/Setup-Linux.sh](Scripts/Setup-Windows.bat)  
- Run `make` in the project root directory