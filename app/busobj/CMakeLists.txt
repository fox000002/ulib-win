SET (busobj_SRCS busobj.cpp busobj.rc)

if (MINGW)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
endif (MINGW)

if (MSVC)
  ADD_DEFINITIONS("-DBUILD_EXE")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
endif (MSVC)

add_executable (busobjx WIN32 ${busobj_SRCS})

target_link_libraries (busobjx uwin uhook)