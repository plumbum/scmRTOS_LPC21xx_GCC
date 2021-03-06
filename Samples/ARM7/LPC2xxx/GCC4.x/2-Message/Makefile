#*****************************************************************
#  makefile for scmRTOS GCC port building
#*****************************************************************

#select preffered toolset or leave commented to use default
#TOOLSET = $(ARM_GCC)/yagarto/20090329
#TOOLSET = $(ARM_GCC)/yagarto/20080408

TARGET = arm-none-eabi-
CC   = $(TARGET)gcc
AS   = $(TARGET)gcc -x assembler-with-cpp
OBJCOPY = $(TARGET)objcopy
OBJDUMP = $(TARGET)objdump 
SIZE = $(TARGET)size 

CHIP = LPC2103

PROJECT = 1-EventFlag

#RUN_MODE = RAM_RUN
RUN_MODE = ROM_RUN

OPTIMIZE = -Os

##############################################################################################
# OS sources directories
scmRTOS_DIRS = ../scmRTOS/Common ../scmRTOS/Ports/ARM7/GCC4.x
# Application sources directories
SRC_DIRS = ./Src ./Startup
# Directories to search includes
INC_DIRS = ./Include
# Directories to search libraries
LIB_DIRS =

##############################################################################################
# THUMB sources
#
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# !!!! sorry, not supported yet !!!!
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#SRC_THUMB_CPP = ./Src/main.cpp
#SRC_THUMB_ASM = 

############################
#  ARM .cpp sources
#       OS sources
SRC_ARM_CPP = OS_Kernel.cpp
SRC_ARM_CPP += OS_Services.cpp
SRC_ARM_CPP += usrlib.cpp
SRC_ARM_CPP += OS_Target_cpp.cpp
SRC_ARM_CPP += system.cpp
#       Application sources
SRC_ARM_CPP += main.cpp
############################
#  ARM .s sources
#       OS sources
SRC_ARM_ASM = OS_Target_asm.s
#       Application sources
SRC_ARM_ASM += crt0.s

############################
# libraries
LIBS =

##############################################################################################

ifeq ($(RUN_MODE),RAM_RUN)
LD_SCRIPT= ./Config/scmRTOS_LPC2103_ram.ld
else
LD_SCRIPT= ./Config/scmRTOS_LPC2103_rom.ld
endif


##############################################################################################
# predefined symbols for all sources
DEFS = -D$(RUN_MODE)
# predefined symbols for .cpp sources
CPP_DEFS = 
# predefined symbols for .s sources
ASM_DEFS = 
##############################################################################################
# Output files
ELF = $(EXEDIR)/$(PROJECT).elf
HEX = $(EXEDIR)/$(PROJECT).hex
BIN = $(EXEDIR)/$(PROJECT).bin
LSS = $(LSTDIR)/$(PROJECT).lss
##############################################################################################
##############################################################################################
MCU  = arm7tdmi

INC_DIRS += $(scmRTOS_DIRS)
INC_DIRS += $(SRC_DIRS)

OUTDIR := release
OBJDIR := $(OUTDIR)/obj
LSTDIR := $(OUTDIR)/lst
EXEDIR := $(OUTDIR)
DEPDIR := $(OUTDIR)/dep
#-----------------------------------------------
#    add surce files directories to search path
vpath
vpath % $(scmRTOS_DIRS)
vpath % $(SRC_DIRS)

OBJS_ARM_ASM  = $(addprefix $(OBJDIR)/,$(notdir $(SRC_ARM_ASM:.s=.o) ))
OBJS_ARM_CPP  = $(addprefix $(OBJDIR)/,$(notdir $(SRC_ARM_CPP:.cpp=.o) ))
OBJS_THUMB_ASM  = $(addprefix $(OBJDIR)/,$(notdir $(SRC_THUMB_ASM:.s=.o) ))
OBJS_THUMB_CPP  = $(addprefix $(OBJDIR)/,$(notdir $(SRC_THUMB_CPP:.cpp=.o) ))
OBJS = $(OBJS_ARM_CPP) $(OBJS_ARM_ASM) $(OBJS_THUMB_CPP) $(OBJS_THUMB_ASM)

# defines
DEFS += -D$(CHIP)=1
CPP_DEFS += $(DEFS)
ASM_DEFS += $(DEFS)
# Flags

FLAGS = -mcpu=$(MCU)
# generate debug info in dwarf-2 format
FLAGS += -g -gdwarf-2
# generate dependency info  
FLAGS += -MD -MP -MF $(DEPDIR)/$(@F).d
# include search path
FLAGS += $(addprefix -I,$(INC_DIRS))

#if any THUMB mode sources, generate interworking code
ifneq ($(strip $(SRC_THUMB_ASM) $(SRC_THUMB_CPP)),)
FLAGS += -mthumb-interwork
endif

ASM_FLAGS = $(FLAGS)
ASM_FLAGS += $(ASM_DEFS)
ASM_FLAGS += -Wa,-amhlds=$(LSTDIR)/$(notdir $(<:.s=.lst)) 

CPP_FLAGS = $(FLAGS)
CPP_FLAGS += $(OPTIMIZE)
CPP_FLAGS += $(CPP_DEFS)
CPP_FLAGS += -fomit-frame-pointer
CPP_FLAGS += -fno-exceptions -fno-rtti
CPP_FLAGS += -Wall -Wextra -Wundef -Wcast-align
CPP_FLAGS += -Wa,-ahlmsdc=$(LSTDIR)/$(notdir $(<:.cpp=.lst)) -fverbose-asm
CPP_FLAGS += -ffunction-sections -fdata-sections    # to remove dead code, if any, at link time
CPP_FLAGS += -Winline -finline-limit=40             # generate warning if inlinign fails
CPP_FLAGS += -funsigned-bitfields -fshort-enums	    # actually not important for scmRTOS


LD_FLAGS = -mcpu=$(MCU)
LD_FLAGS += -nostartfiles
LD_FLAGS += -T$(LD_SCRIPT)
LD_FLAGS += -Wl,-Map=$(LSTDIR)/$(PROJECT).map,--cref,--no-warn-mismatch
LD_FLAGS += $(addprefix -L,$(LIB_DIRS))
LD_FLAGS += $(LIBDIR)
LD_FLAGS += -Wl,--gc-sections


LPC21ISP = ../utils/lpc21isp
LPC21ISP_PORT = /dev/ttyUSB0
LPC21ISP_BAUD = 115200
LPC21ISP_XTAL = 14746
#LPC21ISP_FLASHFILE = $(TARGET).hex
# verbose output:
## LPC21ISP_DEBUG = -debug
# enter bootloader via RS232 DTR/RTS (only if hardware supports this
# feature - see Philips AppNote):
LPC21ISP_CONTROL = -control

#
# makefile rules
#
.SILENT :
.PHONY : all clean tags load
all: directories $(OBJS) $(ELF)  $(LSS) size $(HEX) $(BIN)
#all: directories $(OBJS) $(ELF) $(LSS) size

#all objects depends on makefile
$(OBJS): Makefile

$(OBJS_ARM_CPP) : $(OBJDIR)/%.o : %.cpp
	@echo Compiling: $<
	$(CC) -c $(CPP_FLAGS) $< -o $@

$(OBJS_ARM_ASM) : $(OBJDIR)/%.o : %.s
	@echo Assembling: $<
	$(AS) -c $(ASM_FLAGS) $< -o $@

$(OBJS_THUMB_CPP) : $(OBJDIR)/%.o : %.cpp
	@echo Compiling: $<
	$(CC) -c -mthumb $(CPP_FLAGS) $< -o $@

$(OBJS_THUMB_ASM) : $(OBJDIR)/%.o : %.s
	@echo Assembling: $<
	$(AS) -c  -mthumb $(ASM_FLAGS) $< -o $@

$(ELF): $(OBJS) $(LD_SCRIPT)
	@echo Linking: $@
	$(CC) $(OBJS) $(LD_FLAGS) $(LIBS) -o $@

$(LSS): $(ELF)
	$(OBJDUMP) -h -S -C $< > $@

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

size:
	$(SIZE) -B -t $(ELF)
	$(SIZE) -A -t $(ELF)
	$(CC) --version


$(LPC21ISP):
	@make -C ../utils/

# Program the device.  
load: $(HEX) $(LPC21ISP)
	@echo
	@echo Upload firmware
	$(LPC21ISP) $(LPC21ISP_CONTROL) $(LPC21ISP_DEBUG) $< $(LPC21ISP_PORT) $(LPC21ISP_BAUD) $(LPC21ISP_XTAL)

tags:
	ctags -R --c++-kinds=+p --fields=+iaS --extra=+q . ../../../../../Common ../../../../../Ports

directories:
	mkdir -p $(EXEDIR) $(OBJDIR) $(DEPDIR) $(LSTDIR)
clean:
	rm -rf $(OUTDIR)
	rm -rf tags

    
# if TOOLSET variable set,
# add TOOLSET bin directory first in PATH    
ifneq ($(strip $(TOOLSET)),)
ifeq (,$(findstring ;,$(PATH)))
  PATH := $(subst :,,/$(TOOLSET)/bin):$(PATH)
else
  PATH := $(subst /,\,$(TOOLSET)/bin);$(PATH)
endif
export PATH
endif

# dependencies inclusion
# if make target list not contain this targets - include dependencies
ifeq (,$(findstring clean,$(MAKECMDGOALS)))
 ifeq (,$(findstring directories,$(MAKECMDGOALS)))
-include $(shell mkdir $(DEPDIR) 2>/dev/null) $(wildcard $(DEPDIR)/*.d)
 endif
endif
