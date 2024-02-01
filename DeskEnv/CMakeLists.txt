add_executable(DeskEnv Desk.cpp OfficeConnection.h OfficeConnection.cpp)
target_link_directories(DeskEnv PRIVATE ${SDK_LIBRARY_DIR})
target_link_libraries(DeskEnv PRIVATE ${SDK_LIBRARIES} ${GTK3_LINK_LIBRARIES})

