function(get_dynamic_lib_name _OUT_VAR _NAME)
    if(WIN32)
        set(${_OUT_VAR} "${_NAME}.dll" PARENT_SCOPE)
    elseif(UNIX AND NOT APPLE)
        set(${_OUT_VAR} "lib${_NAME}.so" PARENT_SCOPE)
    elseif(APPLE)
        set(${_OUT_VAR} "lib${_NAME}.dylib" PARENT_SCOPE)
    else()
        message(FATAL_ERROR "Unknown platform. Aborting...")
    endif()
endfunction()