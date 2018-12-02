##########################################################################################
# Geometry Visualization Server
# Copyright (c) 2018 Logan Barnes - All Rights Reserved
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
##########################################################################################
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)

# search directory for protobuf
set(PROTOBUF_IMPORT_DIRS ${CMAKE_CURRENT_LIST_DIR})

# make an output directory
set(PROTO_SRC_DIR ${CMAKE_CURRENT_LIST_DIR})
set(PROTO_DST_DIR ${CMAKE_CURRENT_BINARY_DIR}/protos)
file(MAKE_DIRECTORY ${PROTO_DST_DIR})

function(create_proto_library target_name proto_dir)
    # gather proto files
    file(GLOB_RECURSE GVS_PROTO_FILES
            LIST_DIRECTORIES false
            CONFIGURE_DEPENDS
            ${proto_dir}/*.proto
            )

    # generate service objects
    grpc_generate_cpp(GRPC_SRCS GRPC_HDRS ${PROTO_SRC_DIR} ${PROTO_DST_DIR} ${GVS_PROTO_FILES})

    add_library(${target_name} ${GRPC_HDRS} ${GRPC_SRCS})
    target_link_libraries(${target_name}
            PUBLIC Threads::Threads
            PUBLIC protobuf::libprotobuf
            PUBLIC gRPC::grpc++
            PUBLIC ${CMAKE_DL_LIBS}
            )
    target_include_directories(${target_name} SYSTEM PUBLIC ${PROTO_DST_DIR})
endfunction()

create_proto_library(gvs_protos ${CMAKE_CURRENT_LIST_DIR}/gvs)

if (${GVS_BUILD_TESTS})
    create_proto_library(testing_protos ${CMAKE_CURRENT_LIST_DIR}/testing)
endif ()
