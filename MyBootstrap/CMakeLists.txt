add_executable(MyBootstrap MyBootstrap.cpp)
target_link_directories(MyBootstrap PRIVATE ${SDK_LIBRARY_DIR})
target_link_libraries(MyBootstrap PRIVATE ${SDK_LIBRARIES})
