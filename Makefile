# Makefile for sset2c
# (C)2017 Raphael Kim / rageworx@gmail.com
#

#########################################################################
# About cross compiler, or other platform :
#
# To enable build for embedded linux, you may encomment next 2 lines.
# Or, may need to change your cross compiler path with environments.
# It is up to developer !

# CCPREPATH = ${ARM_LINUX_GCC_PATH}
# CCPREFIX  = arm-linux-

# To enable build for embedded linux, change following line.
# CCPATH    = ${CCPREPATH}/${CCPREFIX}
CCPATH =
#########################################################################

# Compiler configure.
GCC = ${CCPATH}gcc
GPP = ${CCPATH}g++
AR  = ${CCPATH}ar

# Sources and how it built
# Optimization issue: recommend to build with using -ffast-math option.
# Change if your CPU architecture supports more high technologies.
INCDIR    = ./src
SOURCEDIR = ./src
OBJDIR    = ./obj/Release
OUTBIN    = sset2c
OUTDIR    = ./bin/Release
DEFINEOPT = 
OPTIMIZEOPT = -O3 -s -static
CPUARCHOPT =
CFLAGS    = -I$(INCDIR) -I$(SOURCEDIR) -I$(FLTKDIR) $(DEFINEOPT) $(OPTIMIZEOPT) $(CPUARCHOPT) $(BITSOPT) $(FLTKLIBS)

all: prepare clean ${OUTDIR}/${OUTBIN}

prepare:
	@mkdir -p ${OBJDIR}
	@mkdir -p ${OUTDIR}

${OBJDIR}/main.o:
	@echo "Compiling $@ ..."
	@$(GPP) -c ${SOURCEDIR}/main.cpp ${CFLAGS} -o $@

${OUTDIR}/${OUTBIN}: ${OBJDIR}/main.o
	@echo "Generating $@ ..."
	@$(GPP) ${OBJDIR}/*.o ${CFLAGS} -o $@

clean:
	@echo "Cleaning built directories ..."
	@rm -rf ${OBJDIR}/*
	@rm -rf ${OUTDIR}/${OUTBIN}*
