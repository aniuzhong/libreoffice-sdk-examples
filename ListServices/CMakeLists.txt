add_executable(ListServices ListServices.cpp)
target_link_directories(ListServices PRIVATE ${SDK_LIBRARY_DIR})
target_link_libraries(ListServices PRIVATE ${SDK_LIBRARIES})
