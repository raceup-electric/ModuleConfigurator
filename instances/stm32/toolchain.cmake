message(STATUS "Configuring STM32 toolchain")

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(TOOLCHAIN_PREFIX arm-none-eabi)

set(CMAKE_C_COMPILER   ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_OBJCOPY      ${TOOLCHAIN_PREFIX}-objcopy)
set(CMAKE_SIZE         ${TOOLCHAIN_PREFIX}-size)
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_ASM_SOURCE_FILE_EXTENSIONS S ASM s)
set_source_files_properties(
  ${CMAKE_CURRENT_SOURCE_DIR}/*/*.s
  PROPERTIES LANGUAGE ASM
)
set(CMAKE_ASM_FLAGS
    "-mcpu=${MCPU} -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard -x assembler-with-cpp"
)

set(CMAKE_C_FLAGS "-mcpu=${MCPU} -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS "-mcpu=${MCPU} -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard -ffunction-sections -fdata-sections")
set(CMAKE_ASM_FLAGS "-mcpu=${MCPU} -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard -x assembler-with-cpp")
# set(CMAKE_EXE_LINKER_FLAGS "-mcpu=${MCPU} -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard -Wl,--gc-sections")


#add_compile_options(
#  -mcpu=${MCPU}
#  -mthumb
#  -mfloat-abi=hard
#  -mfpu=fpv5-sp-d16
#  -ffunction-sections
#  -fdata-sections
#)

add_link_options(
  -mcpu=${MCPU}
  -mthumb
  -mfloat-abi=hard
  -mfpu=fpv5-sp-d16
  -Wl,--gc-sections
  -T${LINKER_SCRIPT_FOLDER}${STM32_DEVICE}_FLASH.ld
)
