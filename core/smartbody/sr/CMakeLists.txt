add_library(
        sr
        src/sr/sr_alg.cpp
        src/sr/sr_array.cpp
        src/sr/sr_box.cpp
        src/sr/sr_buffer.cpp
        src/sr/sr_color.cpp
        src/sr/sr.cpp
        src/sr/sr_cylinder.cpp
        src/sr/sr_euler.cpp
        src/sr/sr_event.cpp
        src/sr/sr_geo2.cpp
        src/sr/sr_hash_table.cpp
        src/sr/sr_input.cpp
        src/sr/sr_light.cpp
        src/sr/sr_line.cpp
        src/sr/sr_lines.cpp
        src/sr/sr_mat.cpp
        src/sr/sr_material.cpp
        src/sr/sr_model.cpp
        src/sr/sr_model_export_iv.cpp
        src/sr/sr_output.cpp
        src/sr/sr_path_array.cpp
        src/sr/sr_plane.cpp
        src/sr/sr_points.cpp
        src/sr/sr_polygon.cpp
        src/sr/sr_polygons.cpp
        src/sr/sr_quat.cpp
        src/sr/sr_sa_bbox.cpp
        src/sr/sr_sa.cpp
        src/sr/sr_random.cpp
        src/sr/sr_sa_event.cpp
        src/sr/sr_sa_render_mode.cpp
        src/sr/sr_shared_ptr.cpp
        src/sr/sr_sn.cpp
        src/sr/sr_sn_editor.cpp
        src/sr/sr_sn_group.cpp
        src/sr/sr_sn_matrix.cpp
        src/sr/sr_sn_shape.cpp
        src/sr/sr_sphere.cpp
        src/sr/sr_spline.cpp
        src/sr/sr_string_array.cpp
        src/sr/sr_string.cpp
        src/sr/sr_timer.cpp
        src/sr/sr_trackball.cpp
        src/sr/sr_tree.cpp
        src/sr/sr_triangle.cpp
        src/sr/sr_vec2.cpp
        src/sr/sr_vec.cpp
        src/sr/sr_viewer.cpp)

target_include_directories(sr PUBLIC
        src
        )

target_link_libraries(sr PUBLIC
        util
        math)

#if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
#    conan_target_link_libraries(SmartBody)
#endif ()

