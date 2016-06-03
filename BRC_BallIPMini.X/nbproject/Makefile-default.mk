#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/BRC_BallIPMini.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/BRC_BallIPMini.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=src/main.c src/app_host_hid_joypad.c src/brc.c src/slnode11_dspicEP.c src/slcore.c src/usb_config.c src/MPU6050.c src/FutabaServo.c usbframework/usb_hal_dspic33e.c usbframework/usb_host.c usbframework/usb_host_hid.c usbframework/usb_host_hid_parser.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/src/main.o ${OBJECTDIR}/src/app_host_hid_joypad.o ${OBJECTDIR}/src/brc.o ${OBJECTDIR}/src/slnode11_dspicEP.o ${OBJECTDIR}/src/slcore.o ${OBJECTDIR}/src/usb_config.o ${OBJECTDIR}/src/MPU6050.o ${OBJECTDIR}/src/FutabaServo.o ${OBJECTDIR}/usbframework/usb_hal_dspic33e.o ${OBJECTDIR}/usbframework/usb_host.o ${OBJECTDIR}/usbframework/usb_host_hid.o ${OBJECTDIR}/usbframework/usb_host_hid_parser.o
POSSIBLE_DEPFILES=${OBJECTDIR}/src/main.o.d ${OBJECTDIR}/src/app_host_hid_joypad.o.d ${OBJECTDIR}/src/brc.o.d ${OBJECTDIR}/src/slnode11_dspicEP.o.d ${OBJECTDIR}/src/slcore.o.d ${OBJECTDIR}/src/usb_config.o.d ${OBJECTDIR}/src/MPU6050.o.d ${OBJECTDIR}/src/FutabaServo.o.d ${OBJECTDIR}/usbframework/usb_hal_dspic33e.o.d ${OBJECTDIR}/usbframework/usb_host.o.d ${OBJECTDIR}/usbframework/usb_host_hid.o.d ${OBJECTDIR}/usbframework/usb_host_hid_parser.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/src/main.o ${OBJECTDIR}/src/app_host_hid_joypad.o ${OBJECTDIR}/src/brc.o ${OBJECTDIR}/src/slnode11_dspicEP.o ${OBJECTDIR}/src/slcore.o ${OBJECTDIR}/src/usb_config.o ${OBJECTDIR}/src/MPU6050.o ${OBJECTDIR}/src/FutabaServo.o ${OBJECTDIR}/usbframework/usb_hal_dspic33e.o ${OBJECTDIR}/usbframework/usb_host.o ${OBJECTDIR}/usbframework/usb_host_hid.o ${OBJECTDIR}/usbframework/usb_host_hid_parser.o

# Source Files
SOURCEFILES=src/main.c src/app_host_hid_joypad.c src/brc.c src/slnode11_dspicEP.c src/slcore.c src/usb_config.c src/MPU6050.c src/FutabaServo.c usbframework/usb_hal_dspic33e.c usbframework/usb_host.c usbframework/usb_host_hid.c usbframework/usb_host_hid_parser.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/BRC_BallIPMini.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33EP256MU806
MP_LINKER_FILE_OPTION=,--script=p33EP256MU806.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/src/main.o: src/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/main.o.d 
	@${RM} ${OBJECTDIR}/src/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/main.c  -o ${OBJECTDIR}/src/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/main.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -legacy-libc  -O0 -I"src" -I"usbframework" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/app_host_hid_joypad.o: src/app_host_hid_joypad.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/app_host_hid_joypad.o.d 
	@${RM} ${OBJECTDIR}/src/app_host_hid_joypad.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/app_host_hid_joypad.c  -o ${OBJECTDIR}/src/app_host_hid_joypad.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/app_host_hid_joypad.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -legacy-libc  -O0 -I"src" -I"usbframework" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/app_host_hid_joypad.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/brc.o: src/brc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/brc.o.d 
	@${RM} ${OBJECTDIR}/src/brc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/brc.c  -o ${OBJECTDIR}/src/brc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/brc.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -legacy-libc  -O0 -I"src" -I"usbframework" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/brc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/slnode11_dspicEP.o: src/slnode11_dspicEP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/slnode11_dspicEP.o.d 
	@${RM} ${OBJECTDIR}/src/slnode11_dspicEP.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/slnode11_dspicEP.c  -o ${OBJECTDIR}/src/slnode11_dspicEP.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/slnode11_dspicEP.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -legacy-libc  -O0 -I"src" -I"usbframework" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/slnode11_dspicEP.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/slcore.o: src/slcore.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/slcore.o.d 
	@${RM} ${OBJECTDIR}/src/slcore.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/slcore.c  -o ${OBJECTDIR}/src/slcore.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/slcore.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -legacy-libc  -O0 -I"src" -I"usbframework" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/slcore.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/usb_config.o: src/usb_config.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/usb_config.o.d 
	@${RM} ${OBJECTDIR}/src/usb_config.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/usb_config.c  -o ${OBJECTDIR}/src/usb_config.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/usb_config.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -legacy-libc  -O0 -I"src" -I"usbframework" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/usb_config.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/MPU6050.o: src/MPU6050.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/MPU6050.o.d 
	@${RM} ${OBJECTDIR}/src/MPU6050.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/MPU6050.c  -o ${OBJECTDIR}/src/MPU6050.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/MPU6050.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -legacy-libc  -O0 -I"src" -I"usbframework" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/MPU6050.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/FutabaServo.o: src/FutabaServo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/FutabaServo.o.d 
	@${RM} ${OBJECTDIR}/src/FutabaServo.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/FutabaServo.c  -o ${OBJECTDIR}/src/FutabaServo.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/FutabaServo.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -legacy-libc  -O0 -I"src" -I"usbframework" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/FutabaServo.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/usbframework/usb_hal_dspic33e.o: usbframework/usb_hal_dspic33e.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/usbframework" 
	@${RM} ${OBJECTDIR}/usbframework/usb_hal_dspic33e.o.d 
	@${RM} ${OBJECTDIR}/usbframework/usb_hal_dspic33e.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  usbframework/usb_hal_dspic33e.c  -o ${OBJECTDIR}/usbframework/usb_hal_dspic33e.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/usbframework/usb_hal_dspic33e.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -legacy-libc  -O0 -I"src" -I"usbframework" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/usbframework/usb_hal_dspic33e.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/usbframework/usb_host.o: usbframework/usb_host.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/usbframework" 
	@${RM} ${OBJECTDIR}/usbframework/usb_host.o.d 
	@${RM} ${OBJECTDIR}/usbframework/usb_host.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  usbframework/usb_host.c  -o ${OBJECTDIR}/usbframework/usb_host.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/usbframework/usb_host.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -legacy-libc  -O0 -I"src" -I"usbframework" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/usbframework/usb_host.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/usbframework/usb_host_hid.o: usbframework/usb_host_hid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/usbframework" 
	@${RM} ${OBJECTDIR}/usbframework/usb_host_hid.o.d 
	@${RM} ${OBJECTDIR}/usbframework/usb_host_hid.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  usbframework/usb_host_hid.c  -o ${OBJECTDIR}/usbframework/usb_host_hid.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/usbframework/usb_host_hid.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -legacy-libc  -O0 -I"src" -I"usbframework" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/usbframework/usb_host_hid.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/usbframework/usb_host_hid_parser.o: usbframework/usb_host_hid_parser.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/usbframework" 
	@${RM} ${OBJECTDIR}/usbframework/usb_host_hid_parser.o.d 
	@${RM} ${OBJECTDIR}/usbframework/usb_host_hid_parser.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  usbframework/usb_host_hid_parser.c  -o ${OBJECTDIR}/usbframework/usb_host_hid_parser.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/usbframework/usb_host_hid_parser.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -legacy-libc  -O0 -I"src" -I"usbframework" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/usbframework/usb_host_hid_parser.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/src/main.o: src/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/main.o.d 
	@${RM} ${OBJECTDIR}/src/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/main.c  -o ${OBJECTDIR}/src/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/main.o.d"      -mno-eds-warn  -g -omf=elf -legacy-libc  -O0 -I"src" -I"usbframework" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/app_host_hid_joypad.o: src/app_host_hid_joypad.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/app_host_hid_joypad.o.d 
	@${RM} ${OBJECTDIR}/src/app_host_hid_joypad.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/app_host_hid_joypad.c  -o ${OBJECTDIR}/src/app_host_hid_joypad.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/app_host_hid_joypad.o.d"      -mno-eds-warn  -g -omf=elf -legacy-libc  -O0 -I"src" -I"usbframework" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/app_host_hid_joypad.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/brc.o: src/brc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/brc.o.d 
	@${RM} ${OBJECTDIR}/src/brc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/brc.c  -o ${OBJECTDIR}/src/brc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/brc.o.d"      -mno-eds-warn  -g -omf=elf -legacy-libc  -O0 -I"src" -I"usbframework" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/brc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/slnode11_dspicEP.o: src/slnode11_dspicEP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/slnode11_dspicEP.o.d 
	@${RM} ${OBJECTDIR}/src/slnode11_dspicEP.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/slnode11_dspicEP.c  -o ${OBJECTDIR}/src/slnode11_dspicEP.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/slnode11_dspicEP.o.d"      -mno-eds-warn  -g -omf=elf -legacy-libc  -O0 -I"src" -I"usbframework" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/slnode11_dspicEP.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/slcore.o: src/slcore.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/slcore.o.d 
	@${RM} ${OBJECTDIR}/src/slcore.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/slcore.c  -o ${OBJECTDIR}/src/slcore.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/slcore.o.d"      -mno-eds-warn  -g -omf=elf -legacy-libc  -O0 -I"src" -I"usbframework" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/slcore.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/usb_config.o: src/usb_config.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/usb_config.o.d 
	@${RM} ${OBJECTDIR}/src/usb_config.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/usb_config.c  -o ${OBJECTDIR}/src/usb_config.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/usb_config.o.d"      -mno-eds-warn  -g -omf=elf -legacy-libc  -O0 -I"src" -I"usbframework" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/usb_config.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/MPU6050.o: src/MPU6050.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/MPU6050.o.d 
	@${RM} ${OBJECTDIR}/src/MPU6050.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/MPU6050.c  -o ${OBJECTDIR}/src/MPU6050.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/MPU6050.o.d"      -mno-eds-warn  -g -omf=elf -legacy-libc  -O0 -I"src" -I"usbframework" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/MPU6050.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/src/FutabaServo.o: src/FutabaServo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/FutabaServo.o.d 
	@${RM} ${OBJECTDIR}/src/FutabaServo.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  src/FutabaServo.c  -o ${OBJECTDIR}/src/FutabaServo.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/src/FutabaServo.o.d"      -mno-eds-warn  -g -omf=elf -legacy-libc  -O0 -I"src" -I"usbframework" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/src/FutabaServo.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/usbframework/usb_hal_dspic33e.o: usbframework/usb_hal_dspic33e.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/usbframework" 
	@${RM} ${OBJECTDIR}/usbframework/usb_hal_dspic33e.o.d 
	@${RM} ${OBJECTDIR}/usbframework/usb_hal_dspic33e.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  usbframework/usb_hal_dspic33e.c  -o ${OBJECTDIR}/usbframework/usb_hal_dspic33e.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/usbframework/usb_hal_dspic33e.o.d"      -mno-eds-warn  -g -omf=elf -legacy-libc  -O0 -I"src" -I"usbframework" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/usbframework/usb_hal_dspic33e.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/usbframework/usb_host.o: usbframework/usb_host.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/usbframework" 
	@${RM} ${OBJECTDIR}/usbframework/usb_host.o.d 
	@${RM} ${OBJECTDIR}/usbframework/usb_host.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  usbframework/usb_host.c  -o ${OBJECTDIR}/usbframework/usb_host.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/usbframework/usb_host.o.d"      -mno-eds-warn  -g -omf=elf -legacy-libc  -O0 -I"src" -I"usbframework" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/usbframework/usb_host.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/usbframework/usb_host_hid.o: usbframework/usb_host_hid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/usbframework" 
	@${RM} ${OBJECTDIR}/usbframework/usb_host_hid.o.d 
	@${RM} ${OBJECTDIR}/usbframework/usb_host_hid.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  usbframework/usb_host_hid.c  -o ${OBJECTDIR}/usbframework/usb_host_hid.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/usbframework/usb_host_hid.o.d"      -mno-eds-warn  -g -omf=elf -legacy-libc  -O0 -I"src" -I"usbframework" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/usbframework/usb_host_hid.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/usbframework/usb_host_hid_parser.o: usbframework/usb_host_hid_parser.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/usbframework" 
	@${RM} ${OBJECTDIR}/usbframework/usb_host_hid_parser.o.d 
	@${RM} ${OBJECTDIR}/usbframework/usb_host_hid_parser.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  usbframework/usb_host_hid_parser.c  -o ${OBJECTDIR}/usbframework/usb_host_hid_parser.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/usbframework/usb_host_hid_parser.o.d"      -mno-eds-warn  -g -omf=elf -legacy-libc  -O0 -I"src" -I"usbframework" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/usbframework/usb_host_hid_parser.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/BRC_BallIPMini.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/BRC_BallIPMini.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc   -mreserve=data@0x1000:0x101B -mreserve=data@0x101C:0x101D -mreserve=data@0x101E:0x101F -mreserve=data@0x1020:0x1021 -mreserve=data@0x1022:0x1023 -mreserve=data@0x1024:0x1027 -mreserve=data@0x1028:0x104F   -Wl,--local-stack,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,$(MP_LINKER_FILE_OPTION),--heap=2000,--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/BRC_BallIPMini.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/BRC_BallIPMini.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -legacy-libc  -Wl,--local-stack,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--heap=2000,--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}\\xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/BRC_BallIPMini.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf  
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
