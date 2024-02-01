add_executable(UrlResolver UrlResolver.cpp)
target_link_directories(UrlResolver PRIVATE ${SDK_LIBRARY_DIR})
target_link_libraries(UrlResolver PRIVATE ${SDK_LIBRARIES})

add_executable(ObjectLifetime Lifetime/ObjectLifetime.cpp)
target_link_directories(ObjectLifetime PRIVATE ${SDK_LIBRARY_DIR})
target_link_libraries(ObjectLifetime PRIVATE ${SDK_LIBRARIES})
