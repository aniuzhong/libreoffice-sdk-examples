add_executable(Organigram Organigram.cpp)
target_link_directories(Organigram PRIVATE ${SDK_LIBRARY_DIR})
target_link_libraries(Organigram PRIVATE ${SDK_LIBRARIES})

add_executable(PresentationDemo PresentationDemo.cpp)
target_link_directories(PresentationDemo PRIVATE ${SDK_LIBRARY_DIR})
target_link_libraries(PresentationDemo PRIVATE ${SDK_LIBRARIES})
