add_library(weeding)
target_sources(weeding
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/CheckAssignable.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/CheckAssignable.h
        )
target_include_directories(weeding
        PUBLIC
        ${CMAKE_SOURCE_DIR}/src/frontend/ast
        ${CMAKE_SOURCE_DIR}/src/frontend/ast/treetypes
        ${CMAKE_SOURCE_DIR}/src/frontend/prettyprint
        ${CMAKE_SOURCE_DIR}/src/error
        )
target_link_libraries(weeding coverage_config)
