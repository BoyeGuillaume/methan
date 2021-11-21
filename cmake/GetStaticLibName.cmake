function(get_static_lib_name _OUT_VAR _NAME)
    if(WIN32)
        set(${_OUT_VAR} "${_NAME}.lib" PARENT_SCOPE)
    elseif(UNIX)
        set(${_OUT_VAR} "${_NAME}.a" PARENT_SCOPE)
    else()
        message(FATAL_ERROR "Unknown platform. Aborting...")
    endif()
endfunction()