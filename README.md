# KIDSVT - RAM Diagnostics and Error Modulation System

## Project Overview

KIDSVT is a comprehensive RAM diagnostics and error modulation system designed to simulate memory operations, data initialization, and fault detection. The system provides a framework for testing memory using various algorithms with an intuitive graphical interface.

## Key Features

- **Memory Simulation**: Virtual RAM modeling with configurable sizes
- **Data Management**: Memory data initialization and management
- **Fault Injection**: Simulation of various memory faults and errors
- **Testing Language**: Custom domain-specific language for test algorithms

## Testing Algorithms
The system implements three memory testing algorithms:
1. **March Algorithm** - Comprehensive memory testing pattern
2. **Additional Algorithm 1** - [To be specified]
3. **Additional Algorithm 2** - [To be specified]

## User Interface
- **Memory Visualization**: Real-time display of memory contents and status
- **Memory Occupation Tracking**: Visual indication of used/free memory blocks
- **Control Elements**:
  - Algorithm selection combobox
  - Control buttons (Start, Stop, Configure tests)
  - Real-time status display

This project provides a foundation for understanding and implementing memory testing strategies while offering practical tools for RAM diagnostics and error analysis.

please use python 3.13
```
git clone https://github.com/onesShade/KIDSVT_FRONT
cd KIDSVT_FRONT/back
git submodule add -f https://github.com/pybind/pybind11 extern/pybind11
cmake -S . -B build
cmake --build build --config Release
mkdir "..\front\back_pyd" 
copy "build\Release\vram_backend.cp313-win_amd64.pyd" "..\front\back_pyd\vram_backend.cp313-win_amd64.pyd"

cd ../front

py -m pip install pybind11-stubgen PyQt6 pyinstaller
py -m pybind11_stubgen back_pyd.vram_backend --output-dir .
py -m PyInstaller --noconfirm --onedir --windowed --name "KIDSVT_Sim" --add-data "back_pyd;back_pyd" main.py

xcopy "..\back\res" "dist\KIDSVT_Sim\res" /E /I /H
dist\KIDSVT_Sim\KIDSVT_Sim.exe
```