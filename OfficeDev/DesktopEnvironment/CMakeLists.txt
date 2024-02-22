add_executable(DesktopEnvironment Desk.cpp OfficeConnection.h OfficeConnection.cpp)
target_link_directories(DesktopEnvironment PRIVATE ${SDK_LIBRARY_DIR})
target_link_libraries(DesktopEnvironment PRIVATE ${SDK_LIBRARIES} ${GTK3_LINK_LIBRARIES})

