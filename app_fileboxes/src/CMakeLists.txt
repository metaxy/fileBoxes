set(app_fileboxes_SRCS
   mainwindow.cpp
   main.cpp
   fileboxeswidget.cpp
   fileboxicon.cpp
   fileboxwidget.cpp
 )

kde4_add_executable(app_fileboxes ${app_fileboxes_SRCS})

target_link_libraries(app_fileboxes ${KDE4_KDEUI_LIBS} fileboxes)

install(TARGETS app_fileboxes ${INSTALL_TARGETS_DEFAULT_ARGS} )

########### install files ###############

install( FILES app_fileboxes.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
