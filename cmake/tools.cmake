macro(SUBDIRLIST result curdir)
    file(GLOB children RELATIVE ${curdir} ${curdir}/*)
    set(dirlist "")
    foreach(child ${children})
        if(IS_DIRECTORY ${curdir}/${child})
            LIST(APPEND dirlist ${child})
        endif()
    endforeach()
    set(${result} ${dirlist})
endmacro()


macro(COPYQTDLL TARGET_NAME QTTARGETS)
    if (WIN32)
        set(DEBUG_SUFFIX)
        if (CMAKE_BUILD_TYPE MATCHES "Debug")
            set(DEBUG_SUFFIX "d")
        endif ()
        set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
            if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
                set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
            endif ()
        endif ()
        
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${QT_INSTALL_PATH}/plugins"
                "$<TARGET_FILE_DIR:${TARGET_NAME}>/plugins")
        foreach (QT_LIB ${QTTARGETS})
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    "${QT_INSTALL_PATH}/bin/Qt5${QT_LIB}${DEBUG_SUFFIX}.dll"
                    "$<TARGET_FILE_DIR:${TARGET_NAME}>")
        endforeach (QT_LIB)
    endif ()
endmacro()