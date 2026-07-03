# llvm-mingw-clang.cmake

set(LLVM_MINGW_ROOT "C:/Users/user/.utils/llvm-mingw")

set(CMAKE_C_COMPILER
    "${LLVM_MINGW_ROOT}/bin/x86_64-w64-mingw32-clang.exe")

set(CMAKE_CXX_COMPILER
    "${LLVM_MINGW_ROOT}/bin/x86_64-w64-mingw32-clang++.exe")

set(CMAKE_RC_COMPILER
    "${LLVM_MINGW_ROOT}/bin/llvm-rc.exe")

set(CMAKE_AR
    "${LLVM_MINGW_ROOT}/bin/llvm-ar.exe")

set(CMAKE_RANLIB
    "${LLVM_MINGW_ROOT}/bin/llvm-ranlib.exe")

set(CMAKE_NM
    "${LLVM_MINGW_ROOT}/bin/llvm-nm.exe")

set(CMAKE_STRIP
    "${LLVM_MINGW_ROOT}/bin/llvm-strip.exe")