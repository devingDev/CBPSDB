## This file is a quick tutorial on writing CMakeLists for targeting the Vita
cmake_minimum_required(VERSION 2.8)

## This includes the Vita toolchain, must go before project definition
# It is a convenience so you do not have to type
# -DCMAKE_TOOLCHAIN_FILE=$VITASDK/share/vita.toolchain.cmake for cmake. It is
# highly recommended that you include this block for all projects.
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

## Define project parameters here
# Name of the project
project(cbpsdb)
# This line adds Vita helper macros, must go after project definition in order
# to build Vita specific artifacts (self/vpk).
include("${VITASDK}/share/vita.cmake" REQUIRED)

## Configuration options for this app
# Display name (under bubble in LiveArea)
set(VITA_APP_NAME "CBPS DB")
# Unique ID must be exactly 9 characters. Recommended: XXXXYYYYY where X =
# unique string of developer and Y = a unique number for this app
set(VITA_TITLEID  "CBPS00001")
# Optional version string to show in LiveArea's more info screen
set(VITA_VERSION  "01.00")

## Flags and includes for building
# Note that we make sure not to overwrite previous flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fpermissive -Wall -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -std=c++11 -g")
# Optional. You can specify more param.sfo flags this way.
set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -d PARENTAL_LEVEL=1")

find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)

# Add any additional include paths here
include_directories(
  src/minizip/
)

# Add any additional library paths here
# ${CMAKE_CURRENT_BINARY_DIR} lets you use any library currently being built
link_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
)


FUNCTION(ADD_RESOURCES out_var)
  SET(result)
  FOREACH(in_f ${ARGN})
  SET(out_f "${CMAKE_CURRENT_BINARY_DIR}/${in_f}.o")
  GET_FILENAME_COMPONENT(out_dir ${out_f} DIRECTORY)
  ADD_CUSTOM_COMMAND(OUTPUT ${out_f}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${out_dir}
    COMMAND ${CMAKE_LINKER} -r -b binary -o ${out_f} ${in_f}
    DEPENDS ${in_f}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Building resource ${out_f}"
    VERBATIM
    )
  LIST(APPEND result ${out_f})
  ENDFOREACH()
  SET(${out_var} "${result}" PARENT_SCOPE)
ENDFUNCTION()

file(GLOB res_files RELATIVE
  ${CMAKE_SOURCE_DIR}
  resources/default/*.png
  resources/default/*.txt
  resources/electron/*.png
  resources/electron/*.txt
  resources/*.png
  resources/*.txt
  resources/*.bin
  resources/*.suprx
  resources/*.skprx
)


add_resources(vitashell_res ${res_files})

## Build and link
# Add all the files needed to compile here
add_executable(${PROJECT_NAME}
  ${vitashell_res}
  src/minizip/ioapi.c
  src/minizip/unzip.c
  src/minizip/zip.c
  src/zip.cpp
  src/headgen.cpp
  #src/archive.cpp
  src/myfile.cpp
  src/sfo.cpp
  src/strnatcmp.cpp
  src/VitaNet.cpp
  src/VitaPad.cpp
  src/draw_font.cpp
  src/apps.cpp
  src/draw_app_list.cpp
  src/draw_side_menu.cpp
  src/log.cpp
  src/main.cpp
)

# Library to link to (drop the -l prefix). This will mostly be stubs.
target_link_libraries(${PROJECT_NAME}

					vita2d
          ${CURL_LIBRARIES}
          ${OPENSSL_LIBRARIES}
          png
          jpeg
          freetype
          c
          m
          z
          onigmo
          archive
          bz2
          lzma
          crypto
          expat
          taihen_stub
  SceAppMgr_stub
  SceAppUtil_stub
  SceAudio_stub
  SceAudiodec_stub
  SceCamera_stub
  SceCommonDialog_stub
  SceCtrl_stub
  SceDisplay_stub
  SceFios2_stub
  SceGxm_stub
  SceIme_stub
  SceHttp_stub
  SceLibKernel_stub
  SceMtpIfDriver_stub
  SceMusicExport_stub
  SceNet_stub
  SceNetCtl_stub
  SceNpDrm_stub
  SceRegistryMgr_stub
  SceShellSvc_stub
  SceSqlite_stub
  SceSsl_stub
  SceSysmodule_stub
  ScePgf_stub
  ScePhotoExport_stub
  ScePower_stub
  ScePspnetAdhoc_stub
  ScePromoterUtil_stub
  SceTouch_stub
  SceUdcd_stub
  SceUsbstorVStorDriver_stub
  SceVideoExport_stub
  SceVshBridge_stub
)

## Create Vita files
vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME} UNSAFE)
# The FILE directive lets you add additional files to the VPK, the syntax is
# FILE src_path dst_path_in_vpk. In this case, we add the LiveArea paths.
vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
  VERSION ${VITA_VERSION}
  NAME ${VITA_APP_NAME}
  FILE sce_sys/icon0.png sce_sys/icon0.png
  FILE sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
  FILE sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
  FILE sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
  FILE assets/APP_ICON.png assets/APP_ICON.png
  FILE assets/HB_BAR.png assets/HB_BAR.png
  FILE assets/HB_CATAGORY.png assets/HB_CATAGORY.png
  FILE assets/HB_DL_.png assets/HB_DL_.png
  FILE assets/HB_ICON.png assets/HB_ICON.png
  FILE assets/HB_INFO.png assets/HB_INFO.png
  FILE assets/HB_SCROL.png assets/HB_SCROL.png
  FILE assets/HB_SCROL_BAR.png assets/HB_SCROL_BAR.png
  FILE assets/LDFCOMMIUNISMSANS.ttf assets/LDFCOMMIUNISMSANS.ttf
  FILE assets/whitney-bold.ttf assets/whitney-bold.ttf
  FILE assets/System_search_icon.png assets/System_search_icon.png
  FILE assets/PSVita/Vita_Bumper_Right.png assets/PSVita/Vita_Bumper_Right.png
  FILE assets/PSVita/Vita_Dpad.png assets/PSVita/Vita_Dpad.png
  FILE assets/PSVita/Vita_Cross.png assets/PSVita/Vita_Cross.png
  FILE assets/PSVita/Vita_Circle.png assets/PSVita/Vita_Circle.png
  FILE assets/Copyright.png assets/Copyright.png
  FILE assets/default_db.json assets/default_db.json
  FILE resources/head.bin resources/head.bin
)
