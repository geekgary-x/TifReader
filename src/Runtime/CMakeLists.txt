set(TARGET_NAME ${PROJECT_NAME}Runtime)

file(GLOB_RECURSE HEADER_FILES "*.h")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")
file(GLOB_RECURSE UI_FILES "*.ui")
qt5_wrap_ui(UI_HEADERS UI_FILES) 

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES} ${UI_FILES})

add_library(${TARGET_NAME} ${HEADER_FILES} ${SOURCE_FILES} ${UI_FILES})

set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17)
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER ${PROJECT_NAME})

# being a cross-platform target, we enforce standards conformance on MSVC
target_compile_options(${TARGET_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->")
target_compile_options(${TARGET_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")

# Link dependencies    
target_link_libraries(${TARGET_NAME} PUBLIC
Qt5::Core
Qt5::Gui
Qt5::Widgets
          )

target_include_directories(
  ${TARGET_NAME} 
  PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
)
