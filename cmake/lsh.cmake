include(boost)

include_directories(${PROJECT_SOURCE_DIR})

add_library(lsh
        "${PROJECT_SOURCE_DIR}/lsh/bitvector.hpp"
        "${PROJECT_SOURCE_DIR}/lsh/mask.hpp"
        "${PROJECT_SOURCE_DIR}/lsh/table.hpp")
set_target_properties(lsh PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(lsh Boost::dynamic_bitset)