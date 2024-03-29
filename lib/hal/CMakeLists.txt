SET(HAL_LIB hal_${PROJECT_NAME})

SET(HAL_SOURCE
    Src/stm32h7xx_hal_adc.c
    Src/stm32h7xx_hal_adc_ex.c
    Src/stm32h7xx_hal.c
    Src/stm32h7xx_hal_cec.c
    Src/stm32h7xx_hal_cortex.c
    Src/stm32h7xx_hal_dma.c
    Src/stm32h7xx_hal_dma_ex.c
    Src/stm32h7xx_hal_eth.c
    Src/stm32h7xx_hal_eth_ex.c
    Src/stm32h7xx_hal_exti.c
    Src/stm32h7xx_hal_flash.c
    Src/stm32h7xx_hal_flash_ex.c
    Src/stm32h7xx_hal_gpio.c
    Src/stm32h7xx_hal_hsem.c
    Src/stm32h7xx_hal_i2c.c
    Src/stm32h7xx_hal_i2c_ex.c
    Src/stm32h7xx_hal_mdma.c
    Src/stm32h7xx_hal_pcd.c
    Src/stm32h7xx_hal_pcd_ex.c
    Src/stm32h7xx_hal_pwr.c
    Src/stm32h7xx_hal_pwr_ex.c
    Src/stm32h7xx_hal_qspi.c
    Src/stm32h7xx_hal_rcc.c
    Src/stm32h7xx_hal_rcc_ex.c
    Src/stm32h7xx_hal_rng.c
    Src/stm32h7xx_hal_rng_ex.c
    Src/stm32h7xx_hal_rtc.c
    Src/stm32h7xx_hal_rtc_ex.c
    Src/stm32h7xx_hal_sai.c
    Src/stm32h7xx_hal_sai_ex.c
    Src/stm32h7xx_hal_sd.c
    Src/stm32h7xx_hal_sd_ex.c
    Src/stm32h7xx_hal_sdram.c
    Src/stm32h7xx_hal_spdifrx.c
    Src/stm32h7xx_hal_spi.c
    Src/stm32h7xx_hal_spi_ex.c
    Src/stm32h7xx_hal_tim.c
    Src/stm32h7xx_hal_tim_ex.c
    Src/stm32h7xx_hal_uart.c
    Src/stm32h7xx_hal_uart_ex.c
    Src/stm32h7xx_ll_delayblock.c
    Src/stm32h7xx_ll_fmc.c
    Src/stm32h7xx_ll_sdmmc.c
    Src/stm32h7xx_ll_tim.c
    Src/stm32h7xx_ll_usb.c
)

add_library(${HAL_LIB} STATIC ${HAL_SOURCE})

target_link_libraries(${HAL_LIB}  cmsis_${PROJECT_NAME})

target_compile_definitions(${HAL_LIB} PUBLIC -DSTM32H747xx -DUSE_FULL_LL_DRIVER)

#target_compile_definitions(${HAL_LIB} PUBLIC -DUSE_HAL_DRIVER)

target_include_directories(${HAL_LIB}
    SYSTEM
    PUBLIC
    ${TARGET_INCLUDE_CONF}
    Inc/Legacy
    Inc/
)