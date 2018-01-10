# 
set (ULIB_FILENAME uwin)
set (ULIB_HOOKNAME uhook)
include_directories (${ULIB_INC_DIR})

MESSAGE( STATUS "ULIB_INC_DIR:         " ${ULIB_INC_DIR} )

#-----------------------------------------------------------------------------
# 
#
#-----------------------------------------------------------------------------
if (MINGW)
  if (BUILD_SHARED_LIBS)
    ADD_DEFINITIONS("-ansi -Wall -DBUILD_DLL")
  else (BUILD_SHARED_LIBS)
    ADD_DEFINITIONS("-ansi -Wall -DBUILD_STATIC")
  endif(BUILD_SHARED_LIBS)
  IF (ENABLE_DEBUG)
    ADD_DEFINITIONS("-DHU_DEBUG")
  ENDIF(ENABLE_DEBUG)
endif (MINGW)

if (MSVC)
  if (BUILD_SHARED_LIBS)
    ADD_DEFINITIONS("/c  /DBUILD_DLL")
  else (BUILD_SHARED_LIBS)
    ADD_DEFINITIONS("/c /DBUILD_STATIC")
  endif(BUILD_SHARED_LIBS)
  
  IF (ENABLE_DEBUG)
    ADD_DEFINITIONS("/DHU_DEBUG")
  ENDIF(ENABLE_DEBUG)

endif (MSVC)


#-----------------------------------------------------------------------------
# 
#
#-----------------------------------------------------------------------------
set (KENERL_SRCS
    ulib.cpp
)

set (GUI_SRCS 
    umsg.cpp
    ucontrol.cpp
    ustatic.cpp
    uimagelist.cpp
    ubitmap.cpp
    ubutton.cpp
    udialog.cpp
    udialogx.cpp
    uedit.cpp
    uupdownctrl.cpp
    utreeview.cpp
    ulistview.cpp
    ulistbox.cpp
    ucombobox.cpp
    ucomboboxex.cpp
    uanimation.cpp
    uprogressbar.cpp
    ustatusbar.cpp
    ucustomctrl.cpp
    utrackbar.cpp
    umessageonlywindow.cpp
    utabctrl.cpp
    ubasewindow.cpp
    utoolbar.cpp
    usplashwnd.cpp
    uwindowx.cpp
    ucursor.cpp
    ucaret.cpp
    ulink.cpp
    umenu.cpp
    uscrollbar.cpp
    ucommondialog.cpp
    uheader.cpp
    udraglistbox.cpp
    urichedit.cpp
    upanel.cpp
    ulayout.cpp
    uipaddress.cpp
    urebar.cpp
    udatetimepick.cpp
    utooltip.cpp
    uwinapp.cpp
    uhotkey.cpp
    uglwindow.cpp
    ugldialog.cpp
)

set (SYSTEM_SRCS
    uconsole.cpp
    udebug.cpp
    ufileman.cpp
    udllman.cpp
    uobject.cpp
    ugdi.cpp
    uevent.cpp
    ucritical.cpp
    usemaphore.cpp
    umutex.cpp
    utoolhelp.cpp
    ufilemapping.cpp
    umailslot.cpp
    upipe.cpp
    uthunk32.cpp
    uimagehlp.cpp
    usync.cpp
    ufiber.cpp
#    urt.cpp
    utime.cpp
    utimer.cpp
    usys.cpp
    ukeybd.cpp
    uprocess.cpp
    uprocessman.cpp
    uthread.cpp
    untservice.cpp
    uclipboard.cpp
    uregkey.cpp
    ushell.cpp
    umci.cpp
    umixer.cpp
    uiphlp.cpp
    ufile.cpp
    usocket.cpp
)

set (GRAPHIC_SRCS
    uglut.cpp
    udibapi.cpp
    urecttracker.cpp
    uvisualstyle.cpp
    upicture.cpp
    ugdiplus.cpp
    udc.cpp
    ubrush.cpp
    upalette.cpp
    uresource.cpp
    upen.cpp
    ufont.cpp
    uicon.cpp
    uregion.cpp
    uprinter.cpp
)

set (CONFIG_SRCS
    uini.cpp
    uconf.cpp
    ucom.cpp
    uplugin.cpp
    uregex.cpp
    ufiledb.cpp
    ulog.cpp
    udbman.cpp
    ucmdline.cpp
)

set (CRYPTO_SRCS
    md5.cpp
    crc32.cpp
    base64.cpp
    sha.cpp
    des.cpp
)

set (DATABASE_SRCS
    uodbc.cpp
)

set (UHOOK_SRCS
    uhook.cpp
)

set (ULIB_SRCS 
    ${KENERL_SRCS}
    ${GUI_SRCS}
    ${SYSTEM_SRCS}
    ${GRAPHIC_SRCS}
    ${CONFIG_SRCS}
    ${CRYPTO_SRCS}
)


MESSAGE( STATUS "ULIB_FILENAME: " ${ULIB_FILENAME} )

if (BUILD_SHARED_LIBS)
  add_library (${ULIB_FILENAME} SHARED ${ULIB_SRCS})
  target_link_libraries (${ULIB_FILENAME} comctl32 netapi32 psapi iphlpapi shlwapi opengl32 glu32 ws2_32 gdiplus uxtheme winmm) 
else (BUILD_SHARED_LIBS)
   add_library (${ULIB_FILENAME} ${ULIB_SRCS})
endif(BUILD_SHARED_LIBS)

add_library (${ULIB_HOOKNAME} SHARED ${UHOOK_SRCS})
target_link_libraries (${ULIB_HOOKNAME}   uwin)