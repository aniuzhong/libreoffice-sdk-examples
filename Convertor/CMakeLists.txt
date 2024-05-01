add_executable(ImpressPDFExport ImpressPDFExport.cpp)
target_link_directories(ImpressPDFExport PRIVATE ${SDK_LIBRARY_DIR})
target_link_libraries(ImpressPDFExport PRIVATE ${SDK_LIBRARIES})
