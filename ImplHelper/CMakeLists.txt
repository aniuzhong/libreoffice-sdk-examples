add_executable(FileWriter2 FileWriter2.cpp)
target_link_directories(FileWriter2 PRIVATE ${SDK_LIBRARY_DIR})
target_link_libraries(FileWriter2 PRIVATE ${SDK_LIBRARIES})
