project(kio-fileboxes)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include (MacroLibrary)

include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})
add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
set(kio_fileboxes_PART_SRCS kio_fileboxes.cpp)

kde4_add_plugin(kio_fileboxes ${kio_fileboxes_PART_SRCS})

target_link_libraries(kio_fileboxes  ${KDE4_KIO_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_KFILE_LIBS} ${KDE4_SOLID_LIBS}  fileboxes)

install( TARGETS kio_fileboxes  DESTINATION ${PLUGIN_INSTALL_DIR} )
install( FILES fileboxes.protocol  DESTINATION  ${SERVICES_INSTALL_DIR} )

