
# minimum CMake version required for C++20 support, among other things
cmake_minimum_required(VERSION 3.15)

# detect if RaceEngineer is being used as a sub-project of another CMake project
if(NOT DEFINED PROJECT_NAME)
    set(RACEENGINEER_SUBPROJECT OFF)
else()
    set(RACEENGINEER_SUBPROJECT ON)
endif()


SET_SOURCE_FILES_PROPERTIES( src/raceengineer.c PROPERTIES LANGUAGE C)
set(CMAKE_BUILD_TYPE Debug)


project(raceengineer)

set(CMAKE_EXE_LINKER_FLAGS "-Wl,--no-as-needed -ldl")


add_subdirectory(src/raceengineer/gameloop)
add_subdirectory(src/raceengineer/simulatorapi)
add_subdirectory(src/raceengineer/helper)
add_subdirectory(src/raceengineer/slog)

add_executable(raceengineer src/raceengineer/raceengineer.c src/raceengineer/playwav.h src/raceengineer/playwav.c src/raceengineer/metric.h src/raceengineer/metric.c)
target_include_directories(raceengineer PUBLIC config)
target_link_libraries(raceengineer m dl ${LIBUSB_LIBRARY} xdg-basedir ao sndfile argtable2 config gameloop helper slog simulatorapi)


# used for enabling additional compiler options if supported
include(CheckCXXCompilerFlag)

function(enable_cxx_compiler_flag_if_supported flag)
    message(STATUS "[raceengineer] Checking if compiler supports warning flag '${flag}'")
    check_cxx_compiler_flag("${flag}" flag_supported)
    if(flag_supported)
        message(STATUS "[raceengineer] Enabling warning flag '${flag}'")
        target_compile_options(raceengineer INTERFACE "${flag}")
    endif()
    unset(flag_supported CACHE)
endfunction()

# enable a large amount of extra warnings, regardless of build mode
if (MSVC) # MSVC supports different warning options to GCC/Clang
    enable_cxx_compiler_flag_if_supported("/W3")  # set warning level 3
    # if tests are enabled, enable converting all warnings to errors too
    if (ENABLE_TESTS)
        # add_compile_options(/WX)
        enable_cxx_compiler_flag_if_supported("/WX")
    endif()
else() # GCC/Clang warning option
    # NOTE: GCC and Clang support most of the same options, but neither supports all
    # of the others'. By only enabling them if supported, we get graceful failure
    # when trying to enable unsupported flags
    # e.g. at the time of writing, GCC does not support -Wdocumentation
    #
    if(analyze)
        message("-- Analyzer is on")
        target_compile_options(raceengineer PRIVATE -fanalyzer -Wno-analyzer-possible-null-argument -Wno-analyzer-possible-null-dereference)
    endif()
    #target_compile_options(raceengineer PRIVATE -fno-stack-protector)
    # enable all warnings about 'questionable constructs'
    enable_cxx_compiler_flag_if_supported("-Wall")
    # issue 'pedantic' warnings for strict ISO compliance
    enable_cxx_compiler_flag_if_supported("-pedantic")
    # enable 'extra' strict warnings
    enable_cxx_compiler_flag_if_supported("-Wextra")
    # enable sign conversion warnings
    enable_cxx_compiler_flag_if_supported("-Wsign-conversion")
    # enable warnings about mistakes in Doxygen documentation
    enable_cxx_compiler_flag_if_supported("-Wdocumentation")
    # if tests are enabled, enable converting all warnings to errors too
    if (ENABLE_TESTS)
        enable_cxx_compiler_flag_if_supported("-Werror")
        # exclude the following kinds of warnings from being converted into errors
        # unknown-pragma is useful to have as a warning but not as an error, if you have
        # pragmas which are for the consumption of one compiler only
        enable_cxx_compiler_flag_if_supported("-Wno-error=unknown-pragmas")
        # unused variable and function warnings are helpful but we don't need them as errors
        enable_cxx_compiler_flag_if_supported("-Wno-error=unused-function")
        enable_cxx_compiler_flag_if_supported("-Wno-error=unused-variable")
        enable_cxx_compiler_flag_if_supported("-Wno-error=unused-parameter")
        enable_cxx_compiler_flag_if_supported("-Wno-error=unused-private-field")
        enable_cxx_compiler_flag_if_supported("-Wno-error=unused-but-set-variable")
    endif()
endif()

# library
# unit tests --only enable if requested AND we're not building as a sub-project
if(ENABLE_TESTS AND NOT RACEENGINEER_SUBPROJECT)
    message(STATUS "[raceengineer] Unit Tests Enabled")
    add_subdirectory(tests)
    enable_testing()
endif()
