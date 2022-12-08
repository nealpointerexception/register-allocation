project(register-allocation)

# minimum required CMAKE version
CMAKE_MINIMUM_REQUIRED(VERSION 3.7 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# compiler must be 17
set(CMAKE_CXX_STANDARD 17)

# for debugging only
set(CMAKE_BUILD_TYPE Debug)

# required if linking to static library
add_definitions(-DANTLR4CPP_STATIC)

# using /MD flag for antlr4_runtime (for Visual C++ compilers only)
set(ANTLR4_WITH_STATIC_CRT OFF)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Specify the version of the antlr4 library needed for this project.
# By default the latest version of antlr4 will be used.  You can specify a
# specific, stable version by setting a repository tag value or a link
# to a zip file containing the libary source.
# set(ANTLR4_TAG 4.11.1)
# set(ANTLR4_ZIP_REPOSITORY https://github.com/antlr/antlr4/archive/refs/tags/4.11.1.zip)

# add external build for antlrcpp
include(ExternalAntlr4Cpp)
# add antrl4cpp artifacts to project environment
include_directories(${ANTLR4_INCLUDE_DIRS})

# set variable pointing to the antlr tool that supports C++
# this is not required if the jar file can be found under PATH environment
set(ANTLR_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/antlr-4.11.1-complete.jar)
# add macros to generate ANTLR Cpp code from grammar
find_package(ANTLR REQUIRED)

# Call macro to add lexer and grammar to your build dependencies.
antlr_target(SMPLGrammarLexer ${CMAKE_CURRENT_SOURCE_DIR}/grammar/simpleLexer.g4 LEXER
             PACKAGE antlrcpp)
antlr_target(SMPLGrammarParser ${CMAKE_CURRENT_SOURCE_DIR}/grammar/simpleParser.g4 PARSER
	     LISTENER
             PACKAGE antlrcpp
             DEPENDS_ANTLR SMPLGrammarLexer
             DEPENDS ${ANTLR_SMPLGrammarLexer_OUTPUT_DIR}/simpleLexer.tokens
             COMPILE_FLAGS -lib ${ANTLR_SMPLGrammarLexer_OUTPUT_DIR})

# include generated files in project environment
include_directories(${ANTLR_SMPLGrammarLexer_OUTPUT_DIR})
include_directories(${ANTLR_SMPLGrammarParser_OUTPUT_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# add generated grammar to demo binary target
add_executable(reg_alloc 
	        ${CMAKE_SOURCE_DIR}/src/main.cpp
            ${ANTLR_SMPLGrammarLexer_CXX_OUTPUTS}
            ${ANTLR_SMPLGrammarParser_CXX_OUTPUTS}
            ${CMAKE_SOURCE_DIR}/src/ast.cpp
            ${CMAKE_SOURCE_DIR}/src/cfg.cpp
            ${CMAKE_SOURCE_DIR}/src/liveout.cpp
            ${CMAKE_SOURCE_DIR}/src/graph_coloring.cpp
            ${CMAKE_SOURCE_DIR}/src/linear_scan.cpp
            )

target_link_libraries(reg_alloc antlr4_static Threads::Threads)
