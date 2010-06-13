PROJECT(fileboxeshelper)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

kde4_add_executable(fileboxeshelper main.cpp)

target_link_libraries(fileboxeshelper ${KDE4_KDEUI_LIBS} fileboxes)

install(TARGETS fileboxeshelper ${INSTALL_TARGETS_DEFAULT_ARGS} )




