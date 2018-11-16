##########################################################################################
# Copyright (c) 2018 Logan Barnes - All Rights Reserved
# Geometry Visualisation Server
##########################################################################################
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
    target_link_libraries(${target_name} PUBLIC gRPC::grpc++ protobuf::libprotobuf)
    target_include_directories(${target_name} SYSTEM PUBLIC ${PROTO_DST_DIR})
endfunction()

create_proto_library(gvs_protos ${CMAKE_CURRENT_LIST_DIR}/gvs)

if (${GVS_BUILD_TESTS})
    create_proto_library(testing_protos ${CMAKE_CURRENT_LIST_DIR}/testing)
endif ()
