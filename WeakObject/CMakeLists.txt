add_executable(FileWriter1 FileWriter1.cpp)
target_link_directories(FileWriter1 PRIVATE ${SDK_LIBRARY_DIR})
target_link_libraries(FileWriter1 PRIVATE ${SDK_LIBRARIES})
