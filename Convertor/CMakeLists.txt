add_executable(ImpressPDFExport ImpressPDFExport.cpp)
target_link_directories(ImpressPDFExport PRIVATE ${SDK_LIBRARY_DIR})
target_link_libraries(ImpressPDFExport PRIVATE ${SDK_LIBRARIES})

add_executable(writerPDFExport writerPDFExport.cpp)
target_link_directories(writerPDFExport PRIVATE ${SDK_LIBRARY_DIR})
target_link_libraries(writerPDFExport PRIVATE ${SDK_LIBRARIES})

add_executable(calcPDFExport calcPDFExport.cpp)
target_link_directories(calcPDFExport PRIVATE ${SDK_LIBRARY_DIR})
target_link_libraries(calcPDFExport PRIVATE ${SDK_LIBRARIES})