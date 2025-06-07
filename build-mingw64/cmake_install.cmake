# Install script for directory: H:/Devel/CPP/CD2WAV

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/cd_iso_to_wav")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/msys64/mingw64/bin/objdump.exe")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "H:/Devel/CPP/CD2WAV/dist/cd_iso_to_wav.exe")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "H:/Devel/CPP/CD2WAV/dist" TYPE EXECUTABLE FILES "H:/Devel/CPP/CD2WAV/build-mingw64/cd_iso_to_wav.exe")
  if(EXISTS "$ENV{DESTDIR}/H:/Devel/CPP/CD2WAV/dist/cd_iso_to_wav.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/H:/Devel/CPP/CD2WAV/dist/cd_iso_to_wav.exe")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/msys64/mingw64/bin/strip.exe" "$ENV{DESTDIR}/H:/Devel/CPP/CD2WAV/dist/cd_iso_to_wav.exe")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  include("H:/Devel/CPP/CD2WAV/build-mingw64/CMakeFiles/cd_iso_to_wav.dir/install-cxx-module-bmi-Release.cmake" OPTIONAL)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "H:/Devel/CPP/CD2WAV/dist/libcdio-19.dll;H:/Devel/CPP/CD2WAV/dist/libcdio_cdda-2.dll;H:/Devel/CPP/CD2WAV/dist/libcdio_paranoia-2.dll;H:/Devel/CPP/CD2WAV/dist/libcddb-2.dll;H:/Devel/CPP/CD2WAV/dist/libgcc_s_seh-1.dll;H:/Devel/CPP/CD2WAV/dist/libstdc++-6.dll;H:/Devel/CPP/CD2WAV/dist/libiconv-2.dll;H:/Devel/CPP/CD2WAV/dist/libwinpthread-1.dll;H:/Devel/CPP/CD2WAV/dist/libtre-5.dll;H:/Devel/CPP/CD2WAV/dist/libsystre-0.dll;H:/Devel/CPP/CD2WAV/dist/libintl-8.dll")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "H:/Devel/CPP/CD2WAV/dist" TYPE FILE FILES
    "C:/msys64/mingw64/bin/libcdio-19.dll"
    "C:/msys64/mingw64/bin/libcdio_cdda-2.dll"
    "C:/msys64/mingw64/bin/libcdio_paranoia-2.dll"
    "C:/msys64/mingw64/bin/libcddb-2.dll"
    "C:/msys64/mingw64/bin/libgcc_s_seh-1.dll"
    "C:/msys64/mingw64/bin/libstdc++-6.dll"
    "C:/msys64/mingw64/bin/libiconv-2.dll"
    "C:/msys64/mingw64/bin/libwinpthread-1.dll"
    "C:/msys64/mingw64/bin/libtre-5.dll"
    "C:/msys64/mingw64/bin/libsystre-0.dll"
    "C:/msys64/mingw64/bin/libintl-8.dll"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE EXECUTABLE FILES "H:/Devel/CPP/CD2WAV/build-mingw64/cd_iso_to_wav.exe")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/./cd_iso_to_wav.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/./cd_iso_to_wav.exe")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/msys64/mingw64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/./cd_iso_to_wav.exe")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  include("H:/Devel/CPP/CD2WAV/build-mingw64/CMakeFiles/cd_iso_to_wav.dir/install-cxx-module-bmi-Release.cmake" OPTIONAL)
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "H:/Devel/CPP/CD2WAV/build-mingw64/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
if(CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_COMPONENT MATCHES "^[a-zA-Z0-9_.+-]+$")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
  else()
    string(MD5 CMAKE_INST_COMP_HASH "${CMAKE_INSTALL_COMPONENT}")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INST_COMP_HASH}.txt")
    unset(CMAKE_INST_COMP_HASH)
  endif()
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "H:/Devel/CPP/CD2WAV/build-mingw64/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
