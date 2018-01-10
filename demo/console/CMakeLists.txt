SET (console_SRCS test_console.cpp console.rc)

if (MINGW)
  ADD_DEFINITIONS("-DBUILD_EXE")
endif (MINGW)

if (MSVC)
  ADD_DEFINITIONS("-DBUILD_EXE")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
endif (MSVC)

add_executable (test_console ${console_SRCS})
target_link_libraries (test_console uwin)