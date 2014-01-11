# Wind River Workbench generated Makefile.
# Do not edit!!!
#
# The file ".wrmakefile" is the template used by the Wind River Workbench to
# generate the makefiles of this project. Add user-specific build targets and
# make rules only(!) in this project's ".wrmakefile" file. These will then be
# automatically dumped into the makefiles.

WIND_HOME := $(subst \,/,$(WIND_HOME))
WIND_BASE := $(subst \,/,$(WIND_BASE))
WIND_USR := $(subst \,/,$(WIND_USR))
WRVX_COMPBASE := $(subst \,/,$(WRVX_COMPBASE))

all : pre_build main_all post_build

_clean ::
	@echo "make: removing targets and objects of `pwd`"

TRACE=0
TRACEON=$(TRACE:0=@)
TRACE_FLAG=$(TRACEON:1=)

MAKEFILE := Makefile

FLEXIBLE_BUILD := 1

BUILD_SPEC = PPC32diab
DEBUG_MODE = 1
ifeq ($(DEBUG_MODE),1)
MODE_DIR := Debug
else
MODE_DIR := NonDebug
endif
OBJ_DIR := .
WS_ROOT_DIR := C:/WindRiver/workspace
PRJ_ROOT_DIR := $(WS_ROOT_DIR)/Test



#Global Build Macros
PROJECT_TYPE = DKM
DEFINES = 
EXPAND_DBG = 0


#BuildSpec specific Build Macros
VX_CPU_FAMILY = ppc
CPU = PPC32
TOOL_FAMILY = diab
TOOL = diab
TOOL_PATH = 
CC_ARCH_SPEC = -tPPCFH:vxworks63 -Xstmw-slow -DPPC32_fp60x
LIBPATH = 
LIBS = 

IDE_INCLUDES = -I$(WIND_BASE)/target/h -I$(WIND_BASE)/target/h/wrn/coreip 

IDE_LIBRARIES = 

IDE_DEFINES = 



#BuildTool flags
ifeq ($(DEBUG_MODE),1)
DEBUGFLAGS_C-Compiler = -g
DEBUGFLAGS_C++-Compiler = -g
DEBUGFLAGS_Linker = -g
DEBUGFLAGS_Partial-Image-Linker = 
DEBUGFLAGS_Librarian = 
DEBUGFLAGS_Assembler = -g
else
DEBUGFLAGS_C-Compiler = -XO -Xsize-opt
DEBUGFLAGS_C++-Compiler = -XO -Xsize-opt
DEBUGFLAGS_Linker = -XO -Xsize-opt
DEBUGFLAGS_Partial-Image-Linker = 
DEBUGFLAGS_Librarian = 
DEBUGFLAGS_Assembler = -XO -Xsize-opt
endif


#Project Targets
PROJECT_TARGETS = Test/$(MODE_DIR)/Test.out \
	 Test_partialImage/$(MODE_DIR)/Test_partialImage.o


#Rules

# Test
ifeq ($(DEBUG_MODE),1)
Test/$(MODE_DIR)/% : DEBUGFLAGS_C-Compiler = -g
Test/$(MODE_DIR)/% : DEBUGFLAGS_C++-Compiler = -g
Test/$(MODE_DIR)/% : DEBUGFLAGS_Linker = -g
Test/$(MODE_DIR)/% : DEBUGFLAGS_Partial-Image-Linker = 
Test/$(MODE_DIR)/% : DEBUGFLAGS_Librarian = 
Test/$(MODE_DIR)/% : DEBUGFLAGS_Assembler = -g
else
Test/$(MODE_DIR)/% : DEBUGFLAGS_C-Compiler = -XO -Xsize-opt
Test/$(MODE_DIR)/% : DEBUGFLAGS_C++-Compiler = -XO -Xsize-opt
Test/$(MODE_DIR)/% : DEBUGFLAGS_Linker = -XO -Xsize-opt
Test/$(MODE_DIR)/% : DEBUGFLAGS_Partial-Image-Linker = 
Test/$(MODE_DIR)/% : DEBUGFLAGS_Librarian = 
Test/$(MODE_DIR)/% : DEBUGFLAGS_Assembler = -XO -Xsize-opt
endif
Test/$(MODE_DIR)/% : IDE_INCLUDES = -I$(WIND_BASE)/target/h -I$(WIND_BASE)/target/h/wrn/coreip 
Test/$(MODE_DIR)/% : IDE_LIBRARIES = 
Test/$(MODE_DIR)/% : IDE_DEFINES = 
Test/$(MODE_DIR)/% : PROJECT_TYPE = DKM
Test/$(MODE_DIR)/% : DEFINES = 
Test/$(MODE_DIR)/% : EXPAND_DBG = 0
Test/$(MODE_DIR)/% : VX_CPU_FAMILY = ppc
Test/$(MODE_DIR)/% : CPU = PPC32
Test/$(MODE_DIR)/% : TOOL_FAMILY = diab
Test/$(MODE_DIR)/% : TOOL = diab
Test/$(MODE_DIR)/% : TOOL_PATH = 
Test/$(MODE_DIR)/% : CC_ARCH_SPEC = -tPPCFH:vxworks63 -Xstmw-slow -DPPC32_fp60x
Test/$(MODE_DIR)/% : LIBPATH = 
Test/$(MODE_DIR)/% : LIBS = 
Test/$(MODE_DIR)/% : OBJ_DIR := Test/$(MODE_DIR)

OBJECTS_Test = Test_partialImage/$(MODE_DIR)/Test_partialImage.o

Test/$(MODE_DIR)/Test.out : $(OBJECTS_Test)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@";rm -f "$@";ddump -Ng $(OBJECTS_Test) $(IDE_LIBRARIES) | tclsh $(WIND_BASE)/host/resource/hutils/tcl/munch.tcl -c ppc > $(OBJ_DIR)/ctdt.c; $(TOOL_PATH)dcc $(DEBUGFLAGS_Linker) $(CC_ARCH_SPEC) -Xdollar-in-ident -Xforce-declarations $(ADDED_CFLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES)  -o $(OBJ_DIR)/ctdt.o -c $(OBJ_DIR)/ctdt.c; $(TOOL_PATH)dld -tPPCFH:vxworks63 -X -r5 -r4 -o "$@" $(OBJ_DIR)/ctdt.o $(OBJECTS_Test) $(IDE_LIBRARIES) $(LIBPATH) $(LIBS)  $(ADDED_LIBPATH) $(ADDED_LIBS) && if [ "$(EXPAND_DBG)" = "1" ]; then plink "$@";fi

Test/$(MODE_DIR)/Test_compile_file : $(FILE) ;

_clean :: Test/$(MODE_DIR)/Test_clean

Test/$(MODE_DIR)/Test_clean : 
	$(TRACE_FLAG)if [ -d "Test" ]; then cd "Test"; rm -rf $(MODE_DIR); fi


# Test_partialImage
ifeq ($(DEBUG_MODE),1)
Test_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_C-Compiler = -g
Test_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_C++-Compiler = -g
Test_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Linker = -g
Test_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Partial-Image-Linker = 
Test_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Librarian = 
Test_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Assembler = -g
else
Test_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_C-Compiler = -XO -Xsize-opt
Test_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_C++-Compiler = -XO -Xsize-opt
Test_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Linker = -XO -Xsize-opt
Test_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Partial-Image-Linker = 
Test_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Librarian = 
Test_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Assembler = -XO -Xsize-opt
endif
Test_partialImage/$(MODE_DIR)/% : IDE_INCLUDES = -I$(WIND_BASE)/target/h -I$(WIND_BASE)/target/h/wrn/coreip 
Test_partialImage/$(MODE_DIR)/% : IDE_LIBRARIES = 
Test_partialImage/$(MODE_DIR)/% : IDE_DEFINES = 
Test_partialImage/$(MODE_DIR)/% : PROJECT_TYPE = DKM
Test_partialImage/$(MODE_DIR)/% : DEFINES = 
Test_partialImage/$(MODE_DIR)/% : EXPAND_DBG = 0
Test_partialImage/$(MODE_DIR)/% : VX_CPU_FAMILY = ppc
Test_partialImage/$(MODE_DIR)/% : CPU = PPC32
Test_partialImage/$(MODE_DIR)/% : TOOL_FAMILY = diab
Test_partialImage/$(MODE_DIR)/% : TOOL = diab
Test_partialImage/$(MODE_DIR)/% : TOOL_PATH = 
Test_partialImage/$(MODE_DIR)/% : CC_ARCH_SPEC = -tPPCFH:vxworks63 -Xstmw-slow -DPPC32_fp60x
Test_partialImage/$(MODE_DIR)/% : LIBPATH = 
Test_partialImage/$(MODE_DIR)/% : LIBS = 
Test_partialImage/$(MODE_DIR)/% : OBJ_DIR := Test_partialImage/$(MODE_DIR)

Test_partialImage/$(MODE_DIR)/Objects/Test/Main.o : $(PRJ_ROOT_DIR)/Main.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)dcc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -W:c:,-Xclib-optim-off -Xansi -Xlocal-data-area-static-only -Xforce-declarations  -Xmake-dependency=0xd $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


OBJECTS_Test_partialImage = Test_partialImage/$(MODE_DIR)/Objects/Test/Main.o

Test_partialImage/$(MODE_DIR)/Test_partialImage.o : $(OBJECTS_Test_partialImage)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)dld -tPPCFH:vxworks63 -X -r5  -o "$@" $(OBJECTS_Test_partialImage) $(LIBPATH) $(LIBS) $(ADDED_LIBPATH) $(ADDED_LIBS)  && if [ "$(EXPAND_DBG)" = "1" ]; then plink "$@";fi

Test_partialImage/$(MODE_DIR)/Test_partialImage_compile_file : $(FILE) ;

_clean :: Test_partialImage/$(MODE_DIR)/Test_partialImage_clean

Test_partialImage/$(MODE_DIR)/Test_partialImage_clean : 
	$(TRACE_FLAG)if [ -d "Test_partialImage" ]; then cd "Test_partialImage"; rm -rf $(MODE_DIR); fi

force : 

DEP_FILES := Test_partialImage/$(MODE_DIR)/Objects/Test/Main.d
-include $(DEP_FILES)



-include $(PRJ_ROOT_DIR)/*.makefile

-include *.makefile

JOBS?=1
ifeq ($(JOBS),1)
main_all : external_build  $(PROJECT_TARGETS)
	@echo "make: built targets of `pwd`"
else
main_all : external_build 
	@$(MAKE) -f $(MAKEFILE) --jobs $(JOBS) $(MFLAGS) $(PROJECT_TARGETS);\
	echo "make: built targets of `pwd`"
endif 

# entry point for extending the build
external_build ::
	@echo ""

# main entry point for pre processing prior to the build
pre_build :: $(PRE_BUILD_STEP) generate_sources
	@echo ""

# entry point for generating sources prior to the build
generate_sources ::
	@echo ""

# main entry point for post processing after the build
post_build :: $(POST_BUILD_STEP) deploy_output
	@echo ""

# entry point for deploying output after the build
deploy_output ::
	@echo ""

clean :: external_clean $(CLEAN_STEP) _clean

# entry point for extending the build clean
external_clean ::
	@echo ""
