@echo off

where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo [31m[[1;31mERROR[31m]: It doesn't look like you have cmake installed. Please install it and try again.[0m
    exit /b 1
)

cmake -S . -B bin && cmake --build bin --target all -j4 && bin\fbGraphics.exe
