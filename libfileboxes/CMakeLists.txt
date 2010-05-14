find_package(KDE4 REQUIRED)
find_package(Nepomuk REQUIRED)
find_package(Soprano REQUIRED)
include(KDE4Defaults)
 include(SopranoAddOntology)
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES} ${NEPOMUK_INCLUDES} ${SOPRANO_INCLUDE_DIR})
add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
set( fileboxes_LIB_SRCS
  box.cpp
  boxesbackend.cpp
)
set( fileboxes_LIB_HDRS
  fileboxes_export.h
  box.h
  boxesbackend.h
  boxsettings.h
)
#find_file(FILEBOXES_TRIG_SOURCE
   #fileboxes.trig)
#nepomuk_add_ontology_classes(fileboxes_LIB_SRCS ${FILEBOXES_TRIG_SOURCE}  "FILEBOXES"
  # "Nepomuk::Vocabulary"
  #"trig"
  # VISIBILITY "nepomuk")

soprano_add_ontology(fileboxes_LIB_SRCS ${SHAREDDESKTOPONTOLOGIES_ROOT_DIR}/nie/nfo.trig "NFO" "Nepomuk::Vocabulary" "trig")
soprano_add_ontology(fileboxes_LIB_SRCS ${SHAREDDESKTOPONTOLOGIES_ROOT_DIR}/nie/nie.trig "NIE" "Nepomuk::Vocabulary" "trig")
soprano_add_ontology(fileboxes_LIB_SRCS ${SHAREDDESKTOPONTOLOGIES_ROOT_DIR}/nepomuk/nao.trig "NAO" "Soprano::Vocabulary" "trig")
set(GENERIC_LIB_VERSION "0.1")
set(GENERIC_LIB_SOVERSION "0")
kde4_add_library( fileboxes SHARED ${fileboxes_LIB_SRCS} )
 
target_link_libraries( fileboxes
  ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${NEPOMUK_LIBRARIES}  ${SOPRANO_CLIENT_LIBRARIES} ${NEPOMUK_QUERY_LIBRARIES}
  ${SOPRANO_LIBRARIES} ${KDE4_KIO_LIBS}
)
set_target_properties( fileboxes
  PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION}
)
 
install( TARGETS fileboxes  ${INSTALL_TARGETS_DEFAULT_ARGS} ) 
install( FILES ${fileboxes_LIB_HDRS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/fileboxes
  COMPONENT Devel
)
