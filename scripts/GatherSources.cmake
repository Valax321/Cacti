function(gather_sources _dirName _outVariable)
    set(_relativeDirName ${_dirName})
    cmake_path(ABSOLUTE_PATH _dirName NORMALIZE)
    file(STRINGS "${_dirName}/_sources.txt" _sourceFiles)

    foreach(_file IN LISTS _sourceFiles)
        set(_realPath "${_dirName}/${_file}")
        cmake_path(GET _file EXTENSION LAST_ONLY _ext)
        if (${_ext} STREQUAL ".in")
            set(_srcPath ${_realPath})
            cmake_path(REMOVE_EXTENSION _file LAST_ONLY)
            set(_cfgFile "${CMAKE_CURRENT_BINARY_DIR}/${_relativeDirName}/${_file}")
            configure_file(${_realPath} ${_cfgFile} @ONLY)
            set(_realPath ${_cfgFile})
        endif()
        list(APPEND _files ${_realPath})
        set_property(SOURCE ${_realPath} APPEND PROPERTY OBJECT_DEPENDS "${_dirName}/_sources.txt")
    endforeach()

    set(${_outVariable} ${_files} PARENT_SCOPE)
endfunction()
