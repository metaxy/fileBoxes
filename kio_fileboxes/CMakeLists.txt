project(kio-fileboxes)
find_package(KDE4 REQUIRED)
find_package(Nepomuk REQUIRED)
find_package(Soprano REQUIRED)
include (KDE4Defaults)
include (MacroLibrary)
include(SopranoAddOntology)
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES} ${NEPOMUK_INCLUDES} ${SOPRANO_INCLUDE_DIR})
add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
set(kio_fileboxes_PART_SRCS kio_fileboxes.cpp)
soprano_add_ontology(kio_fileboxes_PART_SRCS ${SHAREDDESKTOPONTOLOGIES_ROOT_DIR}/nie/nie.trig "NIE" "Nepomuk::Vocabulary" "trig")

kde4_add_plugin(kio_fileboxes ${kio_fileboxes_PART_SRCS})

target_link_libraries(kio_fileboxes  ${KDE4_KIO_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_KFILE_LIBS} ${KDE4_SOLID_LIBS}  ${NEPOMUK_LIBRARIES}  ${SOPRANO_CLIENT_LIBRARIES} ${NEPOMUK_QUERY_LIBRARIES}
  ${SOPRANO_LIBRARIES}  fileboxes)

install( TARGETS kio_fileboxes  DESTINATION ${PLUGIN_INSTALL_DIR} )
install( FILES fileboxes.protocol  DESTINATION  ${SERVICES_INSTALL_DIR} )

