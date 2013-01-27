# Copyright (c) 2013
#

# Common Dependencies
#if(USCBG_BUILD_CLIENT OR USCBG_BUILD_SERVER)
#  IncludePackage(Bullet)
#  IncludePackage(ENet)
#endif()

# Client Dependencies
if(USCBG_BUILD_CLIENT)
  IncludePackage(SDL)
  IncludePackage(SDL_image)
  IncludePackage(SDL_ttf)
  IncludePackage(OpenGL)
endif()

# Include Subdirectories
include_directories("${PROJECT_SOURCE_DIR}/uscbg/cfg")
include_directories("${PROJECT_SOURCE_DIR}/uscbg/common")
include_directories("${PROJECT_SOURCE_DIR}/uscbg/dlnk")
include_directories("${PROJECT_SOURCE_DIR}/uscbg/hsm")
include_directories("${PROJECT_SOURCE_DIR}/uscbg/net")
include_directories("${PROJECT_SOURCE_DIR}/uscbg/pbuf")
include_directories("${PROJECT_SOURCE_DIR}/uscbg/gfw")
include_directories("${PROJECT_SOURCE_DIR}/uscbg/glx")
include_directories("${PROJECT_SOURCE_DIR}/uscbg/gui")
include_directories("${PROJECT_SOURCE_DIR}/uscbg/scf")
include_directories("${PROJECT_SOURCE_DIR}/uscbg/slnk")
include_directories("${PROJECT_SOURCE_DIR}/uscbg/sys")
include_directories("${PROJECT_SOURCE_DIR}/uscbg/trc")

# Common Subdirectories
if(USCBG_BUILD_CLIENT OR USCBG_BUILD_SERVER)
  add_subdirectory(cfg)
  add_subdirectory(common)
  add_subdirectory(dlnk)
  add_subdirectory(hsm)
  add_subdirectory(net)
  add_subdirectory(pbuf)
  add_subdirectory(scf)
  add_subdirectory(slnk)
  add_subdirectory(trc)
endif()

# Client Subdirectories
if(USCBG_BUILD_CLIENT)
  add_subdirectory(client)
  add_subdirectory(gfw)
  add_subdirectory(glx)
  add_subdirectory(gui)
endif()

# Server Subdirectories
if(USCBG_BUILD_SERVER)
  add_subdirectory(server)
endif()
