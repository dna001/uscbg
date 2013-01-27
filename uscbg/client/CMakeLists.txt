# Copyright (c) 2013
#

# Build the Client

set(USCBG_CLIENT_SRCS
  us.c
  gui_gbwnd.c
  gui_log.c
  gui_logwnd.c
  gui_piwnd.c
  main_hsm.c
  net_client.c
)

add_executable(USClient
  ${USCBG_CLIENT_SRCS}
)

target_link_libraries(USClient
  cfg
  common
  dlnk
  gfw
  glx
  gui
  hsm
  net
  pbuf
  scf
  slnk
  trc
  ${SDL_LIBRARY}
  ${SDLIMAGE_LIBRARY}
  ${SDLTTF_LIBRARY}
  ${OPENGL_LIBRARY}
  SDLmain
  ${WINSOCK_LIB}
)
