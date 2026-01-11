set(SOURCES
    main.cpp
    view/mainwindow.cpp
    view/mainwindow.h
    view/mainwindow.ui
    stringtable.cpp
    stringtable.h
)

add_executable(StringTableEditor ${SOURCES})
target_link_libraries(StringTableEditor PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_include_directories(StringTableEditor
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/view
)
