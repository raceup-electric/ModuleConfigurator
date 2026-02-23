add_library(drivers STATIC
  ${DRIVER_SOURCES}
)

target_include_directories(drivers PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/src
)
