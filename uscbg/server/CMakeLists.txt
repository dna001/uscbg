# Copyright (c) 2013
#

# Build the Server

set(USCBG_SERVER_SRCS
  us_server.c
  server_hsm.c
  net_server.c
)

add_executable(USServer
  ${USCBG_SERVER_SRCS}
)

target_link_libraries(USServer
  cfg
  common
  trc
  dlnk
  hsm
  net
  pbuf
  scf
  slnk
  pthread
  ${WINSOCK_LIB}
)
