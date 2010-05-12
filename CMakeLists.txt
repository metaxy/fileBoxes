project(fileboxes)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)

include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

add_subdirectory( icons )
add_subdirectory( kio_fileboxes )
add_subdirectory( app_fileboxes )
add_subdirectory( plasma_fileboxes )