add_executable(FirstLoadComponent FirstLoadComponent.cpp)
target_link_directories(FirstLoadComponent PRIVATE ${SDK_LIBRARY_DIR})
target_link_libraries(FirstLoadComponent PRIVATE ${SDK_LIBRARIES})
