SET (richedit_SRCS test_richedit.cpp richedit.rc)

if (MINGW)
  ADD_DEFINITIONS("-DBUILD_EXE")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
endif (MINGW)

if (MSVC)
  ADD_DEFINITIONS("-DBUILD_EXE")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
endif (MSVC)

add_executable (test_richedit WIN32 ${richedit_SRCS})

target_link_libraries (test_richedit uwin)
