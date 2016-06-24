################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
core/mllite/data_builder.obj: ../core/mllite/data_builder.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv5/tools/compiler/msp430_4.1.1/bin/cl430" -vmspx --abi=coffabi --code_model=large --data_model=restricted --near_data=globals -O0 --opt_for_speed=5 -g --include_path="C:/ti/ccsv5/ccs_base/msp430/include" --include_path="C:/ti/ccsv5/tools/compiler/msp430_4.1.1/include" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/eMPL" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/include" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430/F5xx_F6xx_Core_Lib" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430/USB_API" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430/USB_eMPL" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/eMPL-hal" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/mllite" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/mpl" --include_path="C:/motion_driver_6_msp430/eMD-6.0/simple_apps/msp430" --gcc --define=__MSP430F5528__ --define=USE_DMP --define=MPL_LOG_NDEBUG=1 --define=CONFIG_INTERFACE_USB --define=MPU9250 --define=I2C_B0 --define=EMPL --define=EMPL_TARGET_MSP430 --diag_warning=225 --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=full --preproc_with_compile --preproc_dependency="core/mllite/data_builder.pp" --obj_directory="core/mllite" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

core/mllite/hal_outputs.obj: ../core/mllite/hal_outputs.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv5/tools/compiler/msp430_4.1.1/bin/cl430" -vmspx --abi=coffabi --code_model=large --data_model=restricted --near_data=globals -O0 --opt_for_speed=5 -g --include_path="C:/ti/ccsv5/ccs_base/msp430/include" --include_path="C:/ti/ccsv5/tools/compiler/msp430_4.1.1/include" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/eMPL" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/include" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430/F5xx_F6xx_Core_Lib" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430/USB_API" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430/USB_eMPL" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/eMPL-hal" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/mllite" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/mpl" --include_path="C:/motion_driver_6_msp430/eMD-6.0/simple_apps/msp430" --gcc --define=__MSP430F5528__ --define=USE_DMP --define=MPL_LOG_NDEBUG=1 --define=CONFIG_INTERFACE_USB --define=MPU9250 --define=I2C_B0 --define=EMPL --define=EMPL_TARGET_MSP430 --diag_warning=225 --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=full --preproc_with_compile --preproc_dependency="core/mllite/hal_outputs.pp" --obj_directory="core/mllite" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

core/mllite/message_layer.obj: ../core/mllite/message_layer.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv5/tools/compiler/msp430_4.1.1/bin/cl430" -vmspx --abi=coffabi --code_model=large --data_model=restricted --near_data=globals -O0 --opt_for_speed=5 -g --include_path="C:/ti/ccsv5/ccs_base/msp430/include" --include_path="C:/ti/ccsv5/tools/compiler/msp430_4.1.1/include" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/eMPL" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/include" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430/F5xx_F6xx_Core_Lib" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430/USB_API" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430/USB_eMPL" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/eMPL-hal" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/mllite" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/mpl" --include_path="C:/motion_driver_6_msp430/eMD-6.0/simple_apps/msp430" --gcc --define=__MSP430F5528__ --define=USE_DMP --define=MPL_LOG_NDEBUG=1 --define=CONFIG_INTERFACE_USB --define=MPU9250 --define=I2C_B0 --define=EMPL --define=EMPL_TARGET_MSP430 --diag_warning=225 --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=full --preproc_with_compile --preproc_dependency="core/mllite/message_layer.pp" --obj_directory="core/mllite" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

core/mllite/ml_math_func.obj: ../core/mllite/ml_math_func.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv5/tools/compiler/msp430_4.1.1/bin/cl430" -vmspx --abi=coffabi --code_model=large --data_model=restricted --near_data=globals -O0 --opt_for_speed=5 -g --include_path="C:/ti/ccsv5/ccs_base/msp430/include" --include_path="C:/ti/ccsv5/tools/compiler/msp430_4.1.1/include" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/eMPL" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/include" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430/F5xx_F6xx_Core_Lib" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430/USB_API" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430/USB_eMPL" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/eMPL-hal" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/mllite" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/mpl" --include_path="C:/motion_driver_6_msp430/eMD-6.0/simple_apps/msp430" --gcc --define=__MSP430F5528__ --define=USE_DMP --define=MPL_LOG_NDEBUG=1 --define=CONFIG_INTERFACE_USB --define=MPU9250 --define=I2C_B0 --define=EMPL --define=EMPL_TARGET_MSP430 --diag_warning=225 --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=full --preproc_with_compile --preproc_dependency="core/mllite/ml_math_func.pp" --obj_directory="core/mllite" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

core/mllite/mlmath.obj: ../core/mllite/mlmath.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv5/tools/compiler/msp430_4.1.1/bin/cl430" -vmspx --abi=coffabi --code_model=large --data_model=restricted --near_data=globals -O0 --opt_for_speed=5 -g --include_path="C:/ti/ccsv5/ccs_base/msp430/include" --include_path="C:/ti/ccsv5/tools/compiler/msp430_4.1.1/include" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/eMPL" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/include" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430/F5xx_F6xx_Core_Lib" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430/USB_API" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430/USB_eMPL" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/eMPL-hal" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/mllite" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/mpl" --include_path="C:/motion_driver_6_msp430/eMD-6.0/simple_apps/msp430" --gcc --define=__MSP430F5528__ --define=USE_DMP --define=MPL_LOG_NDEBUG=1 --define=CONFIG_INTERFACE_USB --define=MPU9250 --define=I2C_B0 --define=EMPL --define=EMPL_TARGET_MSP430 --diag_warning=225 --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=full --preproc_with_compile --preproc_dependency="core/mllite/mlmath.pp" --obj_directory="core/mllite" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

core/mllite/mpl.obj: ../core/mllite/mpl.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv5/tools/compiler/msp430_4.1.1/bin/cl430" -vmspx --abi=coffabi --code_model=large --data_model=restricted --near_data=globals -O0 --opt_for_speed=5 -g --include_path="C:/ti/ccsv5/ccs_base/msp430/include" --include_path="C:/ti/ccsv5/tools/compiler/msp430_4.1.1/include" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/eMPL" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/include" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430/F5xx_F6xx_Core_Lib" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430/USB_API" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430/USB_eMPL" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/eMPL-hal" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/mllite" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/mpl" --include_path="C:/motion_driver_6_msp430/eMD-6.0/simple_apps/msp430" --gcc --define=__MSP430F5528__ --define=USE_DMP --define=MPL_LOG_NDEBUG=1 --define=CONFIG_INTERFACE_USB --define=MPU9250 --define=I2C_B0 --define=EMPL --define=EMPL_TARGET_MSP430 --diag_warning=225 --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=full --preproc_with_compile --preproc_dependency="core/mllite/mpl.pp" --obj_directory="core/mllite" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

core/mllite/results_holder.obj: ../core/mllite/results_holder.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv5/tools/compiler/msp430_4.1.1/bin/cl430" -vmspx --abi=coffabi --code_model=large --data_model=restricted --near_data=globals -O0 --opt_for_speed=5 -g --include_path="C:/ti/ccsv5/ccs_base/msp430/include" --include_path="C:/ti/ccsv5/tools/compiler/msp430_4.1.1/include" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/eMPL" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/include" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430/F5xx_F6xx_Core_Lib" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430/USB_API" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430/USB_eMPL" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/eMPL-hal" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/mllite" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/mpl" --include_path="C:/motion_driver_6_msp430/eMD-6.0/simple_apps/msp430" --gcc --define=__MSP430F5528__ --define=USE_DMP --define=MPL_LOG_NDEBUG=1 --define=CONFIG_INTERFACE_USB --define=MPU9250 --define=I2C_B0 --define=EMPL --define=EMPL_TARGET_MSP430 --diag_warning=225 --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=full --preproc_with_compile --preproc_dependency="core/mllite/results_holder.pp" --obj_directory="core/mllite" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

core/mllite/start_manager.obj: ../core/mllite/start_manager.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv5/tools/compiler/msp430_4.1.1/bin/cl430" -vmspx --abi=coffabi --code_model=large --data_model=restricted --near_data=globals -O0 --opt_for_speed=5 -g --include_path="C:/ti/ccsv5/ccs_base/msp430/include" --include_path="C:/ti/ccsv5/tools/compiler/msp430_4.1.1/include" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/eMPL" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/include" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430/F5xx_F6xx_Core_Lib" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430/USB_API" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430/USB_eMPL" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/eMPL-hal" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/mllite" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/mpl" --include_path="C:/motion_driver_6_msp430/eMD-6.0/simple_apps/msp430" --gcc --define=__MSP430F5528__ --define=USE_DMP --define=MPL_LOG_NDEBUG=1 --define=CONFIG_INTERFACE_USB --define=MPU9250 --define=I2C_B0 --define=EMPL --define=EMPL_TARGET_MSP430 --diag_warning=225 --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=full --preproc_with_compile --preproc_dependency="core/mllite/start_manager.pp" --obj_directory="core/mllite" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

core/mllite/storage_manager.obj: ../core/mllite/storage_manager.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv5/tools/compiler/msp430_4.1.1/bin/cl430" -vmspx --abi=coffabi --code_model=large --data_model=restricted --near_data=globals -O0 --opt_for_speed=5 -g --include_path="C:/ti/ccsv5/ccs_base/msp430/include" --include_path="C:/ti/ccsv5/tools/compiler/msp430_4.1.1/include" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/eMPL" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/include" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430/F5xx_F6xx_Core_Lib" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430/USB_API" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/driver/msp430/USB_eMPL" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/eMPL-hal" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/mllite" --include_path="C:/motion_driver_6_msp430/eMD-6.0/core/mpl" --include_path="C:/motion_driver_6_msp430/eMD-6.0/simple_apps/msp430" --gcc --define=__MSP430F5528__ --define=USE_DMP --define=MPL_LOG_NDEBUG=1 --define=CONFIG_INTERFACE_USB --define=MPU9250 --define=I2C_B0 --define=EMPL --define=EMPL_TARGET_MSP430 --diag_warning=225 --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=full --preproc_with_compile --preproc_dependency="core/mllite/storage_manager.pp" --obj_directory="core/mllite" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


