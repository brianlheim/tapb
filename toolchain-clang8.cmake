set(CMAKE_C_COMPILER clang-8)
set(CMAKE_CXX_COMPILER clang-8)

include_directories(SYSTEM /usr/local/llvm/include/c++/v1)
include_directories(SYSTEM /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include)

link_libraries(-lstdc++)
