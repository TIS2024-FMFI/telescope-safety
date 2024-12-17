# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(WIZNET_LIB_PATH "${CMAKE_SOURCE_DIR}/libraries/ioLibrary_Driver-master")
# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Set board
#set(BOARD_NAME WIZnet_Ethernet_HAT)
#set(BOARD_NAME W5100S_EVB_PICO)
# set(BOARD_NAME W5500_EVB_PICO)
set(BOARD_NAME W55RP20_EVB_PICO)
#set(BOARD_NAME W5100S_EVB_PICO2)
#set(BOARD_NAME W5500_EVB_PICO2)

if(${BOARD_NAME} STREQUAL WIZnet_Ethernet_HAT)
    #set(PICO_PLATFORM rp2350) #If you are using WIZnet_ethernet_HAT with Pico2, uncomment it.
    set(WIZNET_CHIP W5100S)
    add_definitions(-D_WIZCHIP_=W5100S)
    add_definitions(-DDEVICE_BOARD_NAME=WIZnet_Ethernet_HAT)
    
elseif(${BOARD_NAME} STREQUAL W5100S_EVB_PICO)
    set(WIZNET_CHIP W5100S)
    add_definitions(-D_WIZCHIP_=W5100S)
    add_definitions(-DDEVICE_BOARD_NAME=W5100S_EVB_PICO)
    
elseif(${BOARD_NAME} STREQUAL W5500_EVB_PICO)
    set(WIZNET_CHIP W5500)
    add_definitions(-D_WIZCHIP_=W5500)
    add_definitions(-DDEVICE_BOARD_NAME=W5500_EVB_PICO)

elseif(${BOARD_NAME} STREQUAL W55RP20_EVB_PICO)
    set(WIZNET_CHIP W5500)
    add_definitions(-D_WIZCHIP_=W5500)
    add_definitions(-DDEVICE_BOARD_NAME=W55RP20_EVB_PICO)

elseif(${BOARD_NAME} STREQUAL W5100S_EVB_PICO2)
    set(PICO_PLATFORM rp2350)
    set(WIZNET_CHIP W5100S)
    add_definitions(-D_WIZCHIP_=W5100S)
    add_definitions(-DDEVICE_BOARD_NAME=W5100S_EVB_PICO2)
    
elseif(${BOARD_NAME} STREQUAL W5500_EVB_PICO2)
    set(PICO_PLATFORM rp2350)
    set(WIZNET_CHIP W5500)
    add_definitions(-D_WIZCHIP_=W5500)
    add_definitions(-DDEVICE_BOARD_NAME=W5500_EVB_PICO2)
else() 
    message(FATAL_ERROR "BOARD_NAME is wrong = ${BOARD_NAME}")
endif()

message(STATUS "WIZNET_CHIP = ${WIZNET_CHIP}")

if(NOT DEFINED PICO_SDK_PATH)
    set(PICO_SDK_PATH ${CMAKE_SOURCE_DIR}/libraries/pico-sdk)
    message(STATUS "PICO_SDK_PATH = ${PICO_SDK_PATH}")
endif()

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)
# include(wiznet_pico_c_sdk_version.cmake)


project(Ethernet C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Set the project root directory if it's not already defined, as may happen if
# the tests folder is included directly by a parent project, without including
# the top level CMakeLists.txt.
if(NOT DEFINED WIZNET_DIR)
    set(WIZNET_DIR ${CMAKE_SOURCE_DIR}/port/ioLibrary_Driver)
    message(STATUS "WIZNET_DIR = ${WIZNET_DIR}")
endif()

if(NOT DEFINED MBEDTLS_DIR)
    set(MBEDTLS_DIR ${CMAKE_SOURCE_DIR}/port/mbedtls)
    message(STATUS "MBEDTLS_DIR = ${MBEDTLS_DIR}")
endif()

if(NOT DEFINED PORT_DIR)
    set(PORT_DIR ${CMAKE_SOURCE_DIR}/port)
    message(STATUS "PORT_DIR = ${PORT_DIR}")
endif()

# Turn off mbedtls test mode 
set(ENABLE_PROGRAMS OFF CACHE BOOL "Build mbedtls programs")
set(ENABLE_TESTING OFF CACHE BOOL "Build mbedtls testing")
add_definitions(-DMBEDTLS_CONFIG_FILE="${PORT_DIR}/mbedtls/inc/ssl_config.h")
add_definitions(-DSET_TRUSTED_CERT_IN_SAMPLES)


include_directories(${WIZNET_LIB_PATH}/Ethernet)
include_directories(${WIZNET_LIB_PATH}/Internet)
include_directories(${WIZNET_LIB_PATH}/Internet/httpServer)
include_directories(${WIZNET_LIB_PATH}/Internet/DHCP)
include_directories(${WIZNET_LIB_PATH}/Ethernet/W5500)
include_directories(${CMAKE_SOURCE_DIR}/port/ioLibrary_Driver)
include_directories(${CMAKE_SOURCE_DIR}/port/timer)
include_directories(${CMAKE_SOURCE_DIR}/port/ioLibrary_Driver/inc)

file(GLOB WIZNET_SOURCES
    "${WIZNET_LIB_PATH}/Ethernet/*.c"
    "${WIZNET_LIB_PATH}/Internet/*.c"
    "${WIZNET_LIB_PATH}/Internet/httpServer/*.c"
    "${WIZNET_LIB_PATH}/Internet/DHCP/*.c"
    "${WIZNET_LIB_PATH}/Ethernet/W5500/*.c"
    "${CMAKE_SOURCE_DIR}/port/ioLibrary_Driver/src/*.c"
    "${CMAKE_SOURCE_DIR}/port/timer/*.c"
    "${CMAKE_SOURCE_DIR}/*.c"
    # "${CMAKE_SOURCE_DIR}/port/ioLibrary_Driver/*.c"
)




# Add libraries in subdirectories
# add_subdirectory(${CMAKE_SOURCE_DIR}/libraries)
# add_subdirectory(${MBEDTLS_DIR})
add_subdirectory(${PORT_DIR})

# Add executable. Default name is the project name, version 0.1

add_executable(Ethernet Ethernet.c ${WIZNET_SOURCES})

# Add the PIO file and generate the header
pico_generate_pio_header(Ethernet ${CMAKE_SOURCE_DIR}/port/ioLibrary_Driver/src/wiznet_spi_pio.pio)

pico_set_program_name(Ethernet "Ethernet")
pico_set_program_version(Ethernet "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Ethernet 0)
pico_enable_stdio_usb(Ethernet 1)

# Add the standard library to the build
target_link_libraries(
        Ethernet
        pico_stdlib
        pico_multicore
        pico_unique_id
        pico_time
        hardware_spi
        hardware_dma
        hardware_pio
        hardware_flash
        # W5500
        # httpServer
        # ioLibrary_Driver-master
        # DHCP
        )
        
target_sources(Ethernet PRIVATE
    ${WIZNET_SOURCES}
)

# Add the standard include files to the build
target_include_directories(Ethernet PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
#   ${WIZNET_SOURCES}
  ${PORT_DIR}
  )




pico_add_extra_outputs(Ethernet)

message(STATUS "WIZNET_SOURCES = ${WIZNET_SOURCES}")
