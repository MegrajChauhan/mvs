CC = gcc
FLAGS = -Wall -Wextra -MMD -MP -g -fsanitize=address -fno-omit-frame-pointer
DIRS = ../../lib/arch/x86_64 \
      ../../lib/comps \
      ../../lib/include \
      ../../lib/internals \
      ../../lib/utils \
      ../../int \
	  include \
	  include/internal \
	  include/def \
	  include/comp \
	  utils \
	  lib
      
SRC_DIR = src/
INC_DIRS = ${addprefix -I, ${DIRS}}
FLAGS += ${flags}

OUTPUT_DIR = ../../build/
OUTPUT_DEPS= ../../build/

FILES_TO_COMPILE = ${foreach _D, ${SRC_DIR},${wildcard ${_D}*.c}}
FILES_TO_COMPILE += merry.c
OUTPUT_FILES_NAME = ${patsubst %.c, ${OUTPUT_DIR}%.o, ${FILES_TO_COMPILE}}
DEPS=${patsubst %.c, ${OUTPUT_DEPS}%.d, ${FILES_TO_COMPILE}}

all: directories ${OUTPUT_FILES_NAME}
	ar rcs  ${OUTPUT_DIR}libcore64.a ${OUTPUT_FILES_NAME}

WATCH_PROJECT: directories ${OUTPUT_FILES_NAME}

${OUTPUT_DIR}%.o: %.c
	${CC} ${FLAGS} ${INC_DIRS} -c $< -o $@

# Create necessary directories
directories:
	mkdir -p ${OUTPUT_DIR}
	${foreach f, ${SRC_DIR}, ${shell mkdir -p ${OUTPUT_DIR}${f}}}

clean:
	rm -rf ${OUTPUT_DIR}

.PHONY: all clean directories

-include $(DEPS)
