SET (singleton_SRCS test_singleton.cpp)

if (MINGW)
  ADD_DEFINITIONS("-DBUILD_EXE")
endif (MINGW)

if (MSVC)
  ADD_DEFINITIONS("-DBUILD_EXE")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
endif (MSVC)

add_executable (test_singleton ${singleton_SRCS})
