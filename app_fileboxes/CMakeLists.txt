project(app_fileboxes)

find_package(KDE4 REQUIRED)
include (KDE4Defaults)

include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} /usr/include/fileboxes )

add_subdirectory( src )
