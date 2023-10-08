cmake_minimum_required (VERSION 3.9)

project (StateOfMind)

set (CMAKE_C_STANDARD 99)

if (MSVC)
    # ignore the most common uncritical warnings:
    # - 4244 + 4305 = int/float/double conversion
    # - 4101 + 4102 = unreferenced variable/label
    add_compile_options (/wd4244 /wd4305 /wd4101 /wd4102)
    add_compile_definitions (_CRT_SECURE_NO_WARNINGS)
endif ()

# set up SDL on Win32+MSVC
if (MSVC)
    set (SDL_VERSION "2.28.2")  # <-- feel free to change this

    # set paths
    set (SDL_SDK_DIR "${CMAKE_CURRENT_LIST_DIR}/SDL2-${SDL_VERSION}")
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set (ARCH x64)
    else ()
        set (ARCH x86)
    endif ()

    # download SDL if not already present
    if (NOT IS_DIRECTORY "${SDL_SDK_DIR}")
        message (STATUS "SDL2-${SDL_VERSION} not found, downloading and unpacking ...")
        set (SDL_ARCHIVE "SDL2-devel-${SDL_VERSION}-VC.zip")
        if (NOT EXISTS "${SDL_ARCHIVE}")
            file (DOWNLOAD "https://github.com/libsdl-org/SDL/releases/download/release-${SDL_VERSION}/${SDL_ARCHIVE}" "${SDL_ARCHIVE}")
        endif ()
        file (ARCHIVE_EXTRACT INPUT "${SDL_ARCHIVE}" DESTINATION "${CMAKE_CURRENT_LIST_DIR}")
        if (NOT IS_DIRECTORY "${SDL_SDK_DIR}")
            message (ERROR "SDL download and installation failed (expected to find a directory '${SDL_SDK_DIR}' after extracting '${SDL_ARCHIVE}', but it's not there)")
        endif ()
    endif ()

    # ensure that the executable has a suitable SDL2.dll to go with it
    if (NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/SDL2.dll")
        file (COPY "${SDL_SDK_DIR}/lib/${ARCH}/SDL2.dll" DESTINATION "${CMAKE_CURRENT_LIST_DIR}")
    endif ()

    # set up CMake-internal stuff so that find_package() and linking works
    link_directories ("${SDL_SDK_DIR}/lib/${ARCH}")
    set (SDL2_DIR "${SDL_SDK_DIR}/cmake")
endif ()

find_package (SDL2 REQUIRED)
get_target_property (SDL2INC SDL2::SDL2 INTERFACE_INCLUDE_DIRECTORIES)
add_compile_definitions (USE_SDL)

###############################################################################

add_library (gfxlib STATIC
    lib_src/cvrt_col.c
    lib_src/driver.c
    lib_src/mem_map.c
    lib_src/masks.c
    lib_src/drv_col.c
    lib_src/drv_sdl.c
    lib_src/mem.c
    lib_src/msg.c
)
target_include_directories (gfxlib PUBLIC include ${SDL2INC})
target_link_libraries (gfxlib PUBLIC SDL2)

###############################################################################

add_library (audio STATIC
    au_src/audio.c
    au_src/init.c
    au_src/drv_wav.c
    au_src/drv_sdl.c
)
target_include_directories (audio PUBLIC include ${SDL2INC})
target_link_libraries (audio PUBLIC SDL2)

###############################################################################

add_library (lib3d STATIC
    3d_src/node.c
    3d_src/tree.c
    3d_src/world.c
    3d_src/wrld_rsc.c
    3d_src/transf.c
    3d_src/camera.c
    3d_src/light.c
    3d_src/group.c
    3d_src/box.c
    3d_src/mesh.c
    3d_src/vmesh.c
    3d_src/3d_io.c
    3d_src/parts.c
    3d_src/clip.c
    3d_src/shader.c
    3d_src/poly.c
    3d_src/edges.c
    3d_src/flare.c
    3d_src/wld_info.c
    3d_src/quat.c
    3d_src/vista.c
    3d_src/vista2.c
    3d_src/roto.c
    3d_src/frustum.c
    3d_src/move.c
    3d_src/zlight.c
    3d_src/sv_world.c
    3d_src/convex.c
    3d_src/normal.c
    3d_src/optimesh.c
    3d_src/optim2.c
    3d_src/sort.c
    3d_src/anim.c
    3d_src/mk.c
    3d_src/mk2.c
    3d_src/tables.c
    3d_src/render.c
    3d_src/flare16.c
    3d_src/cache/mip_cst.c
    3d_src/cache/mat.c
    3d_src/cache/cache1.c
    3d_src/cache/cache2.c
    3d_src/cache/cache3.c
    3d_src/cache/ld_mat.c
    3d_src/cache/ld_mat2.c
    3d_src/cache/ld_mat2b.c
    3d_src/cache/ld_mat3.c
    3d_src/cache/sv_mat.c
    3d_src/cache/sv_mat2.c
    3d_src/cache/sv_mat3.c
    3d_src/cache/sv_mat4.c
    3d_src/draw8/flare8b.c
    3d_src/draw8/line8.c
    3d_src/draw8/render8.c
    3d_src/draw8/loops28.c
    3d_src/draw16/bilin16.c
    3d_src/draw16/flare16b.c
    3d_src/draw16/line16.c
    3d_src/draw16/render16.c
    3d_src/draw16/loops216.c
    3d_src/draw88/flare88b.c
    3d_src/draw88/line88.c
    3d_src/draw88/render88.c
    3d_src/draw88/loops288.c
    3d_src/mcube/blob.c
    3d_src/mcube/mcube.c
    3d_src/mcube/mcube_a2.c
    3d_src/mcube/mcube3.c
    3d_src/shaders/raster.c
    3d_src/shaders/shadow.c
    3d_src/shaders/vshadow.c
    3d_src/shaders/vshadow2.c
    3d_src/shaders/vshadow3.c
    3d_src/shaders/vshadow4.c
    3d_src/shaders/vshadow5.c
    3d_src/shaders/sh_flat.c
    3d_src/shaders/sh_line.c
    3d_src/shaders/sh_shdw.c
    3d_src/shaders/sh_vshdw.c
    3d_src/shaders/sh_grd.c
    3d_src/shaders/sh_env.c
    3d_src/shaders/sh_uv.c
    3d_src/shaders/sh_uvc.c
    3d_src/shaders/sh_z.c
    3d_src/shaders/sh_z2.c
    3d_src/shaders/sh_bump.c
    3d_src/shaders/line.c
    3d_src/shaders/line2.c
    3d_src/shaders/loops2.c
)
target_include_directories (lib3d PUBLIC include)
target_compile_definitions (lib3d PRIVATE UNIX)
target_compile_definitions (lib3d PRIVATE _SKIP_OPTIM_MESH_)  # avoid mesh optimizer bugs

###############################################################################

add_library (ray STATIC
    ray_src/slabs.c
    ray_src/rendray.c
    ray_src/rlight.c
    ray_src/rmat.c
    ray_src/octree.c
    ray_src/rad.c
    ray_src/rmesh.c
    ray_src/sh_ray.c
)
target_include_directories (ray PUBLIC include)
target_compile_definitions (ray PRIVATE UNIX)

###############################################################################

add_library (bsp STATIC
    bsp_src/bsp.c
    bsp_src/bsp_splt.c
    bsp_src/bsp_tree.c
    bsp_src/bsp_ld.c
    bsp_src/bsp_sv.c
    bsp_src/bsp_cst.c
)
target_include_directories (bsp PUBLIC include)

###############################################################################

add_library (jpg STATIC
    jpg_src/huff.c
    jpg_src/jpegout.c
    jpg_src/markers.c
    jpg_src/ld_jpg.c
    jpg_src/ycc.c
    jpg_src/iycc.c
    jpg_src/ihuff.c
    jpg_src/adct.c
)
target_include_directories (jpg PUBLIC include)
target_compile_definitions (jpg PRIVATE UNIX)

###############################################################################

add_library (mpg STATIC
#    mpg_src/mpeg.c
#    mpg_src/mpeg_pic.c
    mpg_src/mp3_dec.c
    mpg_src/bit_strm.c
    mpg_src/mp3huff.c
    mpg_src/mp3debug.c
    mpg_src/mp3quant.c
    mpg_src/mp3dct.c
    mpg_src/msamples.c
    mpg_src/mp3_strm.c
    mpg_src/mp3_mikm.c
    mpg_src/mp3sgnl.c
    mpg_src/wlt_enc.c
    mpg_src/wlt_dec.c
    mpg_src/wlt_strm.c
)
target_include_directories (mpg PUBLIC include)
target_compile_definitions (mpg PRIVATE UNIX)

###############################################################################

add_library (demo STATIC
    demo_src/blur.c
    demo_src/btm.c
    demo_src/btm_util.c
    demo_src/colfader.c
    demo_src/remapbtm.c
    demo_src/btm_aa.c
    demo_src/io.c
    demo_src/ppm.c
    demo_src/compress.c
    demo_src/ld_btm.c
    demo_src/sv_btm.c
    demo_src/ld_gif.c
    demo_src/sv_gif.c
    demo_src/tga.c
    demo_src/bmp.c
    demo_src/raw.c
    demo_src/sv_any.c
    demo_src/ld_any.c
    demo_src/ld_pak.c
    demo_src/sv_pak.c
    demo_src/zblur.c
    demo_src/truecol.c
    demo_src/lzw.c
    demo_src/lzw_dec.c
    demo_src/lzw_enc.c
    demo_src/lzw_strm.c
    demo_src/cpu.c
    demo_src/bump.c
    demo_src/bump2.c
    demo_src/bump3.c
    demo_src/clamp.c
    demo_src/color.c
    demo_src/global.c
    demo_src/demo_drv.c
    demo_src/music.c
    demo_src/rand.c
    demo_src/pak.c
    demo_src/mem_btm.c
    demo_src/clock.c
    demo_src/timers.c
    demo_src/sprite.c
    demo_src/sprite3.c
    demo_src/spark.c
    demo_src/mix_tab.c
    demo_src/vbuf.c
    demo_src/dmask.c
    demo_src/dmaskout.c
    demo_src/indy.c
)
target_include_directories (demo PUBLIC include ${SDL2INC})
target_compile_definitions (demo PRIVATE UNIX)

###############################################################################

add_library (fx1 STATIC
    dem/fx1/fx1.c
    dem/fx1/fx2.c
    dem/fx1/fade.c
    dem/fx1/scratch.c
    dem/fx1/janim.c
    dem/fx1/mmask.c
    dem/fx1/mix4.c
    dem/fx1/bilin2.c
    dem/fx1/mixdsp.c
)
target_include_directories (fx1 PUBLIC include dem/include)
target_compile_definitions (fx1 PRIVATE UNIX)

###############################################################################

add_executable (mind
    dem/src/demo.c
    dem/src/debug.c
    dem/src/ctrl.c
    dem/src/panim.c
    dem/src/parts.c
    dem/src/part0.c
    dem/src/part1.c
    dem/src/part2.c
    dem/src/part6.c
    dem/src/part10.c
    dem/src/part14.c
    dem/src/appear.c
    dem/src/blb.c
    dem/src/dsp.c
    dem/src/world2.c
    dem/src/world3.c
    dem/src/world4.c
    dem/src/caustics.c
    dem/src/fonts.c
    dem/src/fonts2.c
    dem/src/spans.c
)
target_include_directories (mind PUBLIC include dem/include ${SDL2INC})
target_link_libraries (mind PUBLIC fx1 demo mpg jpg bsp ray lib3d audio gfxlib SDL2main)
target_compile_definitions (mind PRIVATE UNIX)

###############################################################################

if (NOT WIN32)
    find_package (Threads REQUIRED)
    target_link_libraries (mind PUBLIC Threads::Threads m)
else ()
    set_target_properties(mind PROPERTIES WIN32_EXECUTABLE $<CONFIG:Release>)
endif ()

# make the binary appear in the project's root directory
set_target_properties (mind PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY                "${CMAKE_CURRENT_LIST_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${CMAKE_CURRENT_LIST_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${CMAKE_CURRENT_LIST_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "${CMAKE_CURRENT_LIST_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_CURRENT_LIST_DIR}"
    OUTPUT_NAME_DEBUG                       "mind_debug"
)

# acquire mind.dat if it's not present
if (NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/mind.dat")
    message (STATUS "mind.dat not found, downloading and unpacking ...")
    if (NOT EXISTS mind_v12.zip)
        file (DOWNLOAD http://files.scene.org/get/parties/1998/theparty98/demo/b_state_w32_v12.zip mind_v12.zip)
    endif ()
    file (ARCHIVE_EXTRACT INPUT mind_v12.zip DESTINATION "${CMAKE_CURRENT_LIST_DIR}" PATTERNS mind.dat)
endif ()
