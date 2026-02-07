
# Library type
set(NNEWTON_LIB_TYPE STATIC)
if(NNEWTON_BUILD_SHARED)
    set(NNEWTON_LIB_TYPE SHARED)
endif()

# Source files (explicit listing)

set(NNEWTON_SOURCES

"math/nMath.cpp"  

    

    

    

    
)

add_library(nNewton ${NNEWTON_LIB_TYPE} ${NNEWTON_SOURCES} "math/nTransform.cpp")


target_compile_features(nNewton PUBLIC cxx_std_20)


# Includes

target_include_directories(nNewton
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)


# Dependencies

target_link_libraries(nNewton 
    PUBLIC 
        glm::glm)
target_compile_definitions(nNewton PUBLIC   GLM_ENABLE_EXPERIMENTAL)


# Warnings

if(MSVC)
    target_compile_options(nNewton PRIVATE /W4)
else()
    target_compile_options(nNewton PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(NNEWTON_WARNINGS_AS_ERRORS)
    target_compile_options(nNewton PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Werror>
    )
endif()


# Unity build

if(NNEWTON_UNITY_BUILD)
    set_target_properties(nNewton PROPERTIES UNITY_BUILD ON)
endif()


# Properties

set_target_properties(nNewton PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    POSITION_INDEPENDENT_CODE ON
    FOLDER "Libraries"
)


# Install




# Alias

add_library(nNewton::nNewton ALIAS nNewton)
