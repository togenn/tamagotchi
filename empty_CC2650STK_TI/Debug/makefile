################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS

GEN_OPTS__FLAG := --cmd_file="configPkg/compiler.opt" 
GEN_CMDS__FLAG := -l"configPkg/linker.cmd" 

ORDERED_OBJS += \
"./sensors/bmp280.obj" \
"./sensors/hdc1000.obj" \
"./sensors/mpu9250.obj" \
"./sensors/opt3001.obj" \
"./sensors/tmp007.obj" \
"./sources/CC2650STK.obj" \
"./sources/accelData.obj" \
"./sources/project_main.obj" \
"./wireless/CWC_CC2650_154Drv.obj" \
"./wireless/CWC_IntegrTest.obj" \
"./wireless/ERRORS.obj" \
"./wireless/comm_lib.obj" \
"../CC2650STK.cmd" \
$(GEN_CMDS__FLAG) \
-l"C:/ti/tirtos_cc13xx_cc26xx_2_21_00_06/products/cc26xxware_2_24_03_17272/driverlib/bin/ccs/driverlib.lib" \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include sensors/subdir_vars.mk
-include sources/subdir_vars.mk
-include wireless/subdir_vars.mk
-include subdir_rules.mk
-include sensors/subdir_rules.mk
-include sources/subdir_rules.mk
-include wireless/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
empty_CC2650STK_TI.out \

EXE_OUTPUTS__QUOTED += \
"empty_CC2650STK_TI.out" \

BIN_OUTPUTS += \
empty_CC2650STK_TI.hex \

BIN_OUTPUTS__QUOTED += \
"empty_CC2650STK_TI.hex" \


# All Target
all: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "empty_CC2650STK_TI.out"

# Tool invocations
empty_CC2650STK_TI.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: Arm Linker'
	"C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/bin/armcl" -mv7M3 --code_state=16 --float_support=none -me --define=ccs -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi -z -m"empty_CC2650STK_TI.map" --heap_size=0 --stack_size=256 -i"C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/lib" -i"C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/include" --reread_libs --diag_wrap=off --display_error_number --warn_sections --xml_link_info="empty_CC2650STK_TI_linkInfo.xml" --rom_model -o "empty_CC2650STK_TI.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

empty_CC2650STK_TI.hex: $(EXE_OUTPUTS)
	@echo 'Building secondary target: "$@"'
	@echo 'Invoking: Arm Hex Utility'
	"C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/bin/armhex" --memwidth=8 --romwidth=8 --diag_wrap=off --intel -o "empty_CC2650STK_TI.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building secondary target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(GEN_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "sensors\bmp280.obj" "sensors\hdc1000.obj" "sensors\mpu9250.obj" "sensors\opt3001.obj" "sensors\tmp007.obj" "sources\CC2650STK.obj" "sources\accelData.obj" "sources\project_main.obj" "wireless\CWC_CC2650_154Drv.obj" "wireless\CWC_IntegrTest.obj" "wireless\ERRORS.obj" "wireless\comm_lib.obj" 
	-$(RM) "sensors\bmp280.d" "sensors\hdc1000.d" "sensors\mpu9250.d" "sensors\opt3001.d" "sensors\tmp007.d" "sources\CC2650STK.d" "sources\accelData.d" "sources\project_main.d" "wireless\CWC_CC2650_154Drv.d" "wireless\CWC_IntegrTest.d" "wireless\ERRORS.d" "wireless\comm_lib.d" 
	-$(RMDIR) $(GEN_MISC_DIRS__QUOTED)
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

