# Copyright (c) 2013
#

cmake_minimum_required(VERSION 2.6)

project(UrbanSprawl)

set(UrbanSprawl_VERSION_MAJOR 0)
set(UrbanSprawl_VERSION_MINOR 1)
set(UrbanSprawl_VERSION_REV 1)

set(USCBG_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${USCBG_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${USCBG_OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${USCBG_OUTPUT_DIRECTORY})
link_directories(${USCBG_OUTPUT_DIRECTORY})

# ----------( Configuration )----------

option(USCBG_BUILD_CLIENT "Build the Urban Sprawl Client" TRUE)
option(USCBG_BUILD_SERVER "Build the Urban Sprawl Server" TRUE)
#option(USCBG_BUILD_TESTS "Build the Urban Sprawl Tests" FALSE)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall")

if(WIN32)
set(WINSOCK_LIB "ws2_32")
endif(WIN32)

# -------------------------------------

set(USCBG_DEPENDENCY_NAMES "")
macro(IncludePackage name)
  find_package(${name} REQUIRED)
  include_directories(${${name}_INCLUDE_DIRS})

  list(LENGTH ${name}_SHARED_LIBRARIES ${name}_SHARED_LIBRARIES_LEN)

  if(NOT ${name}_SHARED_LIBRARIES_LEN EQUAL 0)
    copy_files("${${name}_SHARED_LIBRARIES}" ${USCBG_OUTPUT_DIRECTORY})
  endif()

  set(USCBG_DEPENDENCY_NAMES ${USCBG_DEPENDENCY_NAMES} ${name})
endmacro()

macro(FinishDependencyChecking)
  set(USCBG_MISSING_DEPENDENCIES "")

  foreach(name ${USCBG_DEPENDENCY_NAMES})
    if(NOT ${name}_FOUND)
      set(USCBG_MISSING_DEPENDENCIES ${USCBG_MISSING_DEPENDENCIES} ${name})
    endif()
  endforeach()

  list(LENGTH USCBG_MISSING_DEPENDENCIES USCBG_MISSING_DEPENDENCIES_LEN)

  if(USCBG_MISSING_DEPENDENCIES_LEN EQUAL 0)
    message(STATUS "")
    message(STATUS "All dependencies are met.")
    message(STATUS "")
  else()
    message(ERROR "")
    message(ERROR " The following dependencies are missing:")

    foreach(name ${USCBG_MISSING_DEPENDENCIES})
      message(ERROR " - ${name}")
    endforeach()

    message(ERROR "")
  endif()
endmacro()

# Copy data files to bin directory
foreach(datafile
data/1-Base-Civ.png
data/1-Base-Com.png
data/1-Base-Ind.png
data/1-Base-Park.png
data/1-Base-Res.png
data/1-Civ.png
data/1-Com.png
data/1-Ind.png
data/1-Park.png
data/1-Res.png
data/2-Base-Civ.png
data/2-Base-Com.png
data/2-Base-Ind.png
data/2-Base-Park.png
data/2-Base-Res.png
data/2-civ.png
data/2-com.png
data/2-ind.png
data/2-park.png
data/2-res.png
data/3-Base-Civ.png
data/3-Base-Com.png
data/3-Base-Ind.png
data/3-Base-Park.png
data/3-Base-Res.png
data/3-civ.png
data/3-com.png
data/3-ind.png
data/3-park.png
data/3-res.png
data/4-Base-Civ.png
data/4-Base-Com.png
data/4-Base-Ind.png
data/4-Base-Park.png
data/4-Base-Res.png
data/4-civ.png
data/4-com.png
data/4-ind.png
data/4-park.png
data/4-res.png
data/block_black.png
data/block_green.png
data/block_pink.png
data/block_white.png
data/election_button_outline.png
data/election_button_spinner.png
data/election_button_town_layer_off.png
data/election_button_town_layer_on.png
data/Help.png
data/LMarker-Active_Player.png
data/LMarker-Extra_Favor.png
data/Map-B.png
data/Map-Discard.png
data/Map-D.png
data/Map-H.png
data/Map.jpg
data/Politician-Contractor.png
data/Politician-District_Attorney.png
data/Politician-Mayor.png
data/Politician-Police_Chief.png
data/Politician-Treasurer.png
data/Politician-Union_Boss.png
data/Round-10.png
data/Round-11.png
data/Round-12.png
data/Round-4.png
data/Round-5.png
data/Round-6.png
data/Round-7.png
data/Round-8.png
data/Round-9.png
data/Round-Hi.png
data/Round-Lo.png
data/Urb_cardback_City_FINAL.png
data/Urb_cardback_Metro_FINAL.png
data/Urb_cardback_Plan_FINAL.png
data/Urb_cardback_Town_FINAL.png
data/Urb_card_City01_FINAL.png
data/Urb_card_City02_FINAL.png
data/Urb_card_City03_FINAL.png
data/Urb_card_City04_FINAL.png
data/Urb_card_City05_FINAL.png
data/Urb_card_City06_FINAL.png
data/Urb_card_City07_FINAL.png
data/Urb_card_City08_FINAL.png
data/Urb_card_City09_FINAL.png
data/Urb_card_City10_FINAL.png
data/Urb_card_City11_FINAL.png
data/Urb_card_City12_FINAL.png
data/Urb_card_City13_FINAL.png
data/Urb_card_City14_FINAL.png
data/Urb_card_City15_FINAL.png
data/Urb_card_City16_FINAL.png
data/Urb_card_City17_FINAL.png
data/Urb_card_City18_FINAL.png
data/Urb_card_City19_FINAL.png
data/Urb_card_City20_FINAL.png
data/Urb_card_City21_FINAL.png
data/Urb_card_City22_FINAL.png
data/Urb_card_City23_FINAL.png
data/Urb_card_City24_FINAL.png
data/Urb_card_City25_FINAL.png
data/Urb_card_City26_FINAL.png
data/Urb_card_City27_FINAL.png
data/Urb_card_City28_FINAL.png
data/Urb_card_City29_FINAL.png
data/Urb_card_City30_FINAL.png
data/Urb_card_City31_FINAL.png
data/Urb_card_City32_FINAL.png
data/Urb_card_City33_FINAL.png
data/Urb_card_City34_FINAL.png
data/Urb_card_City35_FINAL.png
data/Urb_card_City36_FINAL.png
data/Urb_card_City37_FINAL.png
data/Urb_card_Metro01_FINAL.png
data/Urb_card_Metro02_FINAL.png
data/Urb_card_Metro03_FINAL.png
data/Urb_card_Metro04_FINAL.png
data/Urb_card_Metro05_FINAL.png
data/Urb_card_Metro06_FINAL.png
data/Urb_card_Metro07_FINAL.png
data/Urb_card_Metro08_FINAL.png
data/Urb_card_Metro09_FINAL.png
data/Urb_card_Metro10_FINAL.png
data/Urb_card_Metro11_FINAL.png
data/Urb_card_Metro12_FINAL.png
data/Urb_card_Metro13_FINAL.png
data/Urb_card_Metro14_FINAL.png
data/Urb_card_Metro15_FINAL.png
data/Urb_card_Metro16_FINAL.png
data/Urb_card_Metro17_FINAL.png
data/Urb_card_Metro18_FINAL.png
data/Urb_card_Metro19_FINAL.png
data/Urb_card_Metro20_FINAL.png
data/Urb_card_Metro21_FINAL.png
data/Urb_card_Metro22_FINAL.png
data/Urb_card_Metro23_FINAL.png
data/Urb_card_Metro24_FINAL.png
data/Urb_card_Metro25_FINAL.png
data/Urb_card_Metro26_FINAL.png
data/Urb_card_Metro27_FINAL.png
data/Urb_card_Metro28_FINAL.png
data/Urb_card_Metro29_FINAL.png
data/Urb_card_Metro30_FINAL.png
data/Urb_card_Metro31_FINAL.png
data/Urb_card_Metro32_FINAL.png
data/Urb_card_Metro33_FINAL.png
data/Urb_card_Metro34_FINAL.png
data/Urb_card_Metro35_FINAL.png
data/Urb_card_Metro36_FINAL.png
data/Urb_card_Metro37_FINAL.png
data/Urb_card_Plan01_FINAL.png
data/Urb_card_Plan02_FINAL.png
data/Urb_card_Plan03_FINAL.png
data/Urb_card_Plan04_FINAL.png
data/Urb_card_Plan05_FINAL.png
data/Urb_card_Plan06_FINAL.png
data/Urb_card_Plan07_FINAL.png
data/Urb_card_Plan08_FINAL.png
data/Urb_card_Plan09_FINAL.png
data/Urb_card_Plan10_FINAL.png
data/Urb_card_Plan11_FINAL.png
data/Urb_card_Plan12_FINAL.png
data/Urb_card_Plan13_FINAL.png
data/Urb_card_Plan14_FINAL.png
data/Urb_card_Plan15_FINAL.png
data/Urb_card_Plan16_FINAL.png
data/Urb_card_Plan17_FINAL.png
data/Urb_card_Plan18_FINAL.png
data/Urb_card_Plan19_FINAL.png
data/Urb_card_Plan20_FINAL.png
data/Urb_card_Plan21_FINAL.png
data/Urb_card_Plan22_FINAL.png
data/Urb_card_Plan23_FINAL.png
data/Urb_card_Plan24_FINAL.png
data/Urb_card_Plan25_FINAL.png
data/Urb_card_Plan26_FINAL.png
data/Urb_card_Plan27_FINAL.png
data/Urb_card_Plan28_FINAL.png
data/Urb_card_Plan29_FINAL.png
data/Urb_card_Plan30_FINAL.png
data/Urb_card_Plan31_FINAL.png
data/Urb_card_Plan32_FINAL.png
data/Urb_card_Plan33_FINAL.png
data/Urb_card_Plan34_FINAL.png
data/Urb_card_Plan35_FINAL.png
data/Urb_card_Plan36_FINAL.png
data/Urb_card_Plan37_FINAL.png
data/Urb_card_Plan38_FINAL.png
data/Urb_card_Plan39_FINAL.png
data/Urb_card_Plan40_FINAL.png
data/Urb_card_Plan41_FINAL.png
data/Urb_card_Plan42_FINAL.png
data/Urb_card_Plan43_FINAL.png
data/Urb_card_Plan44_FINAL.png
data/Urb_card_Plan45_FINAL.png
data/Urb_card_Plan46_FINAL.png
data/Urb_card_Plan47_FINAL.png
data/Urb_card_Plan48_FINAL.png
data/Urb_card_Plan49_FINAL.png
data/Urb_card_Plan50_FINAL.png
data/Urb_card_Plan51_FINAL.png
data/Urb_card_Plan52_FINAL.png
data/Urb_card_Plan53_FINAL.png
data/Urb_card_Plan54_FINAL.png
data/Urb_card_Town01_FINAL.png
data/Urb_card_Town02_FINAL.png
data/Urb_card_Town03_FINAL.png
data/Urb_card_Town04_FINAL.png
data/Urb_card_Town05_FINAL.png
data/Urb_card_Town06_FINAL.png
data/Urb_card_Town07_FINAL.png
data/Urb_card_Town08_FINAL.png
data/Urb_card_Town09_FINAL.png
data/Urb_card_Town10_FINAL.png
data/Urb_card_Town11_FINAL.png
data/Urb_card_Town12_FINAL.png
data/Urb_card_Town13_FINAL.png
data/Urb_card_Town14_FINAL.png
data/Urb_card_Town15_FINAL.png
data/Urb_card_Town16_FINAL.png
data/Urb_card_Town17_FINAL.png
data/Urb_card_Town18_FINAL.png
data/Urb_card_Town19_FINAL.png
data/Urb_card_Town20_FINAL.png
data/Urb_card_Town21_FINAL.png
data/Urb_card_Town22_FINAL.png
data/Urb_card_Town23_FINAL.png
data/Urb_card_Town24_FINAL.png
data/Urb_card_Town25_FINAL.png
data/Urb_card_Town26_FINAL.png
data/Urb_card_Town27_FINAL.png
data/Urb_card_Town28_FINAL.png
data/Urb_card_Town29_FINAL.png
data/Urb_card_Town30_FINAL.png
data/Urb_card_Town31_FINAL.png
data/Urb_card_Town32_FINAL.png
data/Urb_card_Town33_FINAL.png
data/Urb_card_Town34_FINAL.png
data/Urb_card_Town35_FINAL.png
data/Urb_card_Town36_FINAL.png
data/Urb_card_Town37_FINAL.png
data/Urb_Money10_FINAL.png
data/Urb_Money1_FINAL.png
data/Urb_Money20_FINAL.png
data/Urb_Money5_FINAL.png
data/us.png
data/Vocation-education.png
data/Vocation-energy.png
data/Vocation-factory.png
data/Vocation-finance.png
data/Vocation-media.png
data/Vocation-public.png
data/Vocation-tour.png
data/Vocation-transport.png
fonts/eufm10.ttf
fonts/cour.ttf
fonts/times.ttf
us.ini)
  CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/${datafile} ${PROJECT_BINARY_DIR}/${datafile} COPYONLY)
endforeach(datafile)

add_subdirectory(uscbg)
#add_subdirectory(Tests)
