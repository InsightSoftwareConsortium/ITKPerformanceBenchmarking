@call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
@"C:\Program Files\Git\git-bash.exe" -i
REM -i gives us the nice git bash console (useful on Windows 7)
REM From within git-bash we can now invoke evaluate-itk-performance.py

REM Alternatively, we could directly invoke it

REM @cd ..
REM @"C:\Program Files\Git\git-bash.exe" REM ITKPerformanceBenchmarking/evaluate-itk-performance.py
REM pause
