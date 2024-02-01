add_executable(SimpleWindow SimpleWindow.cpp)
target_link_directories(SimpleWindow PRIVATE ${SDK_LIBRARY_DIR})
target_link_libraries(SimpleWindow PRIVATE ${SDK_LIBRARIES})
