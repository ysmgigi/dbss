#---------------------------------------------------------------------
#   author: mali
#     date: 2014-7-11 
# describe: a versatile Makefile
#
#       ps: language type (c/cpp)
#       cc: compiler (gcc/g++)
#    FLAGS: c/c++ language flags (for example: -g -wall -o2)
#     DEST: output file name 
#     LIBS: library name (for example: pthread)
# INCLUDES: header file directory (for example: ./include)
#  SUB_DIR: subdirectory (for example: dir1 dir1/aaa dir/bbb dir2) 
# DEST_DIR: prefix install directory   
#  INSTALL: install directory (actually install to $DEST_DIR/$INSTALL)  
#----------------------------------------------------------------------

#----------------------configure in this part
PS = c
CC = gcc 
#FLAGS = -Wall -g -O2 -Wextra -std=gnu99 -fno-strict-aliasing -fno-tree-pre -Wno-unused-parameter
FLAGS = -Wall -g -O2 -Wextra -std=gnu99 -fno-strict-aliasing -fno-tree-pre -Wno-unused-parameter -DDEBUG_STDOUT
DEST := dbss
LIBS :=  pthread clntsh
LIBSDIR := $(ORACLE_HOME)
INCLUDES := include src/iniparser src/ocilib-4.2.1/include $(ORACLE_HOME)/sdk/include
SUB_DIR := src src/iniparser src/ocilib-4.2.1/src
DEST_DIR := 
INSTALL := /usr/local/bin

#----------------------do nothing in this part
RM := rm -f
FLAGS += -MMD -MF $(patsubst ./%, %, $(patsubst %.o, %.d, $(dir $@).$(notdir $@))) $(addprefix -I, $(INCLUDES))
CFLAGS  += $(FLAGS)
CPPFLAGS  += $(FLAGS)
SRCS := $(wildcard *.$(PS) $(addsuffix /*.$(PS), $(SUB_DIR)))
OBJS := $(patsubst %.$(PS), %.o, $(SRCS))
DEPS := $(patsubst %.$(PS), %.d, $(foreach n,$(SRCS),$(patsubst ./%, %, $(dir $n).$(notdir $n))))
MISS := $(filter-out $(wildcard $(DEPS)), $(DEPS))

all: $(DEST)

clean :
	@$(RM) $(OBJS) 
	@$(RM) $(DEPS) 
	@$(RM) $(DEST)

install:
	@if [ ! -d $(DEST_DIR)$(INSTALL) ]; then mkdir -p $(DEST_DIR)$(INSTALL); fi
	cp -f $(DEST) $(DEST_DIR)$(INSTALL) 
	
ifneq ($(MISS),)
$(MISS):
	@$(RM) $(patsubst %.d, %.o, $(dir $@)$(patsubst .%,%, $(notdir $@)))
endif

-include $(DEPS)

$(DEST): $(OBJS)
	$(CC) -o $(DEST) $(OBJS) $(addprefix -l,$(LIBS)) $(addprefix -L,$(LIBSDIR))

