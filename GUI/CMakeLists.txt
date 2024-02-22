add_executable(MessageBox MessageBox.cpp)
target_link_directories(MessageBox PRIVATE ${SDK_LIBRARY_DIR})
target_link_libraries(MessageBox PRIVATE ${SDK_LIBRARIES})

add_executable(SystemDialog SystemDialog.cpp)
target_link_directories(SystemDialog PRIVATE ${SDK_LIBRARY_DIR})
target_link_libraries(SystemDialog PRIVATE ${SDK_LIBRARIES})
