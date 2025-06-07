cmake_minimum_required(VERSION 3.10)
project(cd_iso_to_wav)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

# Kein statisches Linking
set(CMAKE_EXE_LINKER_FLAGS "")

# Suchpfade für MSYS2 MinGW
set(CMAKE_PREFIX_PATH "C:/msys64/mingw64")
set(ENV{PKG_CONFIG_PATH} "C:/msys64/mingw64/lib/pkgconfig")

# Compiler-Flags
if (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /DNDEBUG")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -s -DNDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
endif()

# Icon-Ressource für Windows einbinden
if (WIN32)
    set(WINDOWS_ICON resource.rc)
endif()

# Quellen
add_executable(cd_iso_to_wav
    main.cpp
    cdda_reader.cpp
    wav_writer.cpp
    cddb_lookup.cpp
    ${WINDOWS_ICON}
)

# Bibliotheken via pkg-config
find_package(PkgConfig REQUIRED)

pkg_check_modules(CDIO REQUIRED libcdio)
pkg_check_modules(PARANOIA REQUIRED libcdio_paranoia)
pkg_check_modules(CDDB REQUIRED libcddb)

# Header- und Linkpfade
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
)

# Windows-Installation
if (WIN32)
    set(DIST_DIR "${CMAKE_SOURCE_DIR}/dist")

    # Installiere EXE direkt nach dist/
    install(TARGETS cd_iso_to_wav DESTINATION "${DIST_DIR}")

    # DLLs nach dist kopieren
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


# Install-Regel (optional, z. B. für cmake --install)
install(TARGETS cd_iso_to_wav DESTINATION .)
