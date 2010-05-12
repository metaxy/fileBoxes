project(plasma-fileboxes)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})
set(fileboxes_SRCS plasmafileboxes.cpp filebox.cpp fileboxicon.cpp)
kde4_add_ui_files(fileboxes_SRCS fileBoxesConfig.ui)
qt4_automoc(${fileboxes_SRCS}) 
kde4_add_plugin(plasma_applet_fileboxes ${fileboxes_SRCS})


target_link_libraries(plasma_applet_fileboxes fileboxes
                      ${KDE4_PLASMA_LIBS}  ${KDE4_KDEUI_LIBS})
 
install(TARGETS plasma_applet_fileboxes
        DESTINATION ${PLUGIN_INSTALL_DIR})
 
install(FILES plasma-applet-fileboxes.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
