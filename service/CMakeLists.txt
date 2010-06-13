project(fileboxes-service)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

install(FILES fileboxes-service.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})