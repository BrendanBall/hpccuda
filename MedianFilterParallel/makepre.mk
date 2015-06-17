CC=g++
NVCC=nvcc

CFLAGS=-m64 -fopenmp -lineinfo -std=c++11
# -std=c++11 #had to take this out cos it doesn’t work on hex =/
LFLAGS=

NVCCFLAGS=-m64 -Xcompiler -fopenmp -Xcompiler -lineinfo --generate-line-info
NVCLFLAGS=-link -lineinfo --generate-line-info

OMPLFLAGS=-lgomp

CUDA_PATH ?= /usr/local/cuda

CUDA_INC_PATH  ?= $(CUDA_PATH)/include
CUDA_LIB_PATH  ?= $(CUDA_PATH)/lib64
CUDA_BIN_PATH  ?= $(CUDA_PATH)/bin

ifeq ($(mode),release)
  DEBUG=0
endif

ifeq ($(mode),RELEASE)
  DEBUG=0
endif

ifeq ($(mode),DEBUG)
  DEBUG=1
endif

ifeq ($(mode),debug)
  DEBUG=1
endif

ifeq ($(DEBUG),1)
  CFLAGS+=-O0 -ggdb -g -DDEBUG
  NVCCFLAGS +=-O0 -G -g
  LFLAGS += 
  TARGET=debug
else
  CFLAGS+=-O3
  NVCCFLAGS += -O3 -DFLOAT -DFORUM -use_fast_math -prec-div=false -ftz=true -prec-sqrt=false --fmad=true 
  TARGET=release
endif

PWD := $(shell pwd)
SUBDIRS := $(wildcard */)
UTILDIR ?= $(PWD)/../../common


# CUDA libs
LFLAGS+=-L$(CUDA_LIB_PATH) -lcudart
      
export CUDACOMPILE=1

# Common includes and paths for CUDA
INCLUDES      := -I$(shell echo $INCLUDE) -I$(CUDA_INC_PATH) -I. -I.. -I$(UTILDIR)

BIN=$(TARGET)/$(BINNAME) 

CPPSRCS = $(wildcard *.cpp)
CPPOBJS = $(patsubst %.cpp,$(TARGET)/%.o, $(CPPSRCS))

CSRCS = $(wildcard *.c)
COBJS = $(patsubst %.c,$(TARGET)/%.o, $(CSRCS))

NVSRCS = $(wildcard *.cu)
NVOBJS = $(patsubst %.cu,$(TARGET)/%.o, $(NVSRCS))

UTULS = $(wildcard $(UTILDIR)/*.cpp)
UTULSF = $(notdir $(UTULS))
UTILOBJS = $(patsubst %.cpp,$(UTILDIR)/$(TARGET)/%.o, $(UTULSF))
UTULSRCS = $(addprefix $(UTILDIR)/, $(UTULSF) )