################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
wireless/%.obj: ../wireless/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/bin/armcl" -mv7M3 --code_state=16 --float_support=none -me -O1 --opt_for_speed=3 --include_path="C:/Users/same/Documents/Yliopisto/Tietokonejarjestelmat/tamagotchi/empty_CC2650STK_TI/hardware_headers" --include_path="C:/Users/same/Documents/Yliopisto/Tietokonejarjestelmat/tamagotchi/empty_CC2650STK_TI/sensors" --include_path="C:/Users/same/Documents/Yliopisto/Tietokonejarjestelmat/tamagotchi/empty_CC2650STK_TI/headers" --include_path="C:/Users/same/Documents/Yliopisto/Tietokonejarjestelmat/tamagotchi/empty_CC2650STK_TI/sensors" --include_path="C:/Users/same/Documents/Yliopisto/Tietokonejarjestelmat/tamagotchi/empty_CC2650STK_TI/wireless" --include_path="C:/Users/same/Documents/Yliopisto/Tietokonejarjestelmat/tamagotchi/empty_CC2650STK_TI" --include_path="C:/Users/same/Documents/Yliopisto/Tietokonejarjestelmat/tamagotchi/empty_CC2650STK_TI" --include_path="C:/ti/tirtos_cc13xx_cc26xx_2_21_00_06/products/cc26xxware_2_24_03_17272" --include_path="C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/include" --define=ccs -g --c11 --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="wireless/$(basename $(<F)).d_raw" --obj_directory="wireless" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


