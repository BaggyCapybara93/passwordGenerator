function(check_exit expected)
    execute_process(
        COMMAND "${GENERATOR}" ${ARGN}
        RESULT_VARIABLE status
        OUTPUT_VARIABLE output
        ERROR_VARIABLE error
    )
    if(NOT "${status}" STREQUAL "${expected}")
        message(FATAL_ERROR "Expected exit ${expected}, got ${status} for ${ARGN}. stderr: ${error}")
    endif()
    if(expected EQUAL 0)
        if("${output}" STREQUAL "" OR NOT "${error}" STREQUAL "")
            message(FATAL_ERROR "Successful command did not produce clean stdout: ${ARGN}")
        endif()
    else()
        if("${error}" STREQUAL "")
            message(FATAL_ERROR "Failed command did not explain the error on stderr: ${ARGN}")
        endif()
    endif()
endfunction()

check_exit(0 --help)
check_exit(0 -h)
check_exit(0 --quiet --seed 42)
check_exit(2 --length 0)
check_exit(2 --unknown-option)
check_exit(1 --quiet --length 1 --custom-chars A --no-lowercase --no-digits --no-special --blacklist "{A}")
