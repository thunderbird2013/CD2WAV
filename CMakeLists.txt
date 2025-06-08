cmake_minimum_required(VERSION 3.10)
project(cd_iso_to_wav)

set(CMAKE_CXX_STANDARD 17)

# Compiler-Flags je nach Build-Typ
if (CMAKE_BUILD_TYPE MATCHES "Release")
    message(STATUS "🔧 Release-Build aktiviert")
    if (MSVC)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /DNDEBUG")
    else()
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -s -DNDEBUG")
    endif()
elseif(CMAKE_BUILD_TYPE MATCHES "Debug")
    message(STATUS "🐞 Debug-Build aktiviert")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
endif()

# Kein statisches Linking erzwingen
set(CMAKE_EXE_LINKER_FLAGS "")

# MSYS2 MinGW Suchpfade (werden von build.sh vorausgesetzt)
set(CMAKE_PREFIX_PATH "C:/msys64/mingw64")
set(ENV{PKG_CONFIG_PATH} "C:/msys64/mingw64/lib/pkgconfig")

# Icon-Ressource für Windows
if (WIN32)
    set(WINDOWS_ICON resource.rc)
endif()

# Quellcode-Dateien
add_executable(cd_iso_to_wav
    main.cpp
    cdda_reader.cpp
    wav_writer.cpp
    cddb_lookup.cpp
    crash_handler.cpp
    ${WINDOWS_ICON}
)

# Bibliotheken über pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(CDIO REQUIRED libcdio)
pkg_check_modules(PARANOIA REQUIRED libcdio_paranoia)
pkg_check_modules(CDDB REQUIRED libcddb)

# Includes und Linkpfade
target_include_directories(cd_iso_to_wav PRIVATE
    ${CDIO_INCLUDE_DIRS}
    ${PARANOIA_INCLUDE_DIRS}
    ${CDDB_INCLUDE_DIRS}
)

target_link_directories(cd_iso_to_wav PRIVATE
    ${CDIO_LIBRARY_DIRS}
    ${PARANOIA_LIBRARY_DIRS}
    ${CDDB_LIBRARY_DIRS}
)

target_link_libraries(cd_iso_to_wav
    ${CDIO_LIBRARIES}
    ${PARANOIA_LIBRARIES}
    ${CDDB_LIBRARIES}
    dbghelp
    ws2_32
)

# Windows-Installation (DLLs in dist kopieren)
if (WIN32)
    set(DIST_DIR "${CMAKE_SOURCE_DIR}/dist")

    install(TARGETS cd_iso_to_wav DESTINATION "${DIST_DIR}")

    install(FILES
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
        DESTINATION "${DIST_DIR}"
    )
endif()

# Install-Regel für cmake --install
install(TARGETS cd_iso_to_wav DESTINATION .)
