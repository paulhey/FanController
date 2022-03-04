################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
main.obj: ../main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/etc/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.4/bin/cl430" -vmsp --abi=eabi --use_hw_mpy=none --include_path="C:/etc/ti/ccsv6/ccs_base/msp430/include" --include_path="C:/etc/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.4/include" --advice:power="1,3,4,6,7,8,9,10,11,12,13,14,15" -g --define=__MSP430G2553__ --diag_warning=225 --diag_wrap=off --display_error_number --printf_support=minimal --preproc_with_compile --preproc_dependency="main.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

sample.obj: ../sample.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/etc/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.4/bin/cl430" -vmsp --abi=eabi --use_hw_mpy=none --include_path="C:/etc/ti/ccsv6/ccs_base/msp430/include" --include_path="C:/etc/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.4/include" --advice:power="1,3,4,6,7,8,9,10,11,12,13,14,15" -g --define=__MSP430G2553__ --diag_warning=225 --diag_wrap=off --display_error_number --printf_support=minimal --preproc_with_compile --preproc_dependency="sample.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


