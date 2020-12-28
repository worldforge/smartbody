add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/protocols/sbmotion.pb.cc
        COMMAND ${CMAKE_COMMAND} -E echo "Building Protobuffer bindings"
        COMMAND ${PROTOC_CMD} --proto_path=. --cpp_out=. sbmotion.proto
        COMMAND ${PROTOC_CMD} --proto_path=. --cpp_out=. sbutilities.proto
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/protocols
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/protocols/sbmotion.proto
)


add_library(
        SmartBody-serialize

        src/sb/SBAssetHandlerSk.cpp
        src/sb/SBAssetHandlerSkm.cpp
        src/sb/SBAssetHandlerAsf.cpp
        src/sb/SBAssetHandlerAmc.cpp
        src/sb/SBAssetHandlerBvh.cpp
        src/sb/SBAssetHandlerSkmb.cpp

        src/sb/SBSerializer.cpp

        src/protocols/sbmotion.pb.cc
)

target_include_directories(SmartBody-serialize PUBLIC
        src
        )


target_link_libraries(SmartBody-serialize PUBLIC
        SmartBody

        )
