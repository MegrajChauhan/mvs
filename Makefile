# Variable definitions
CC = gcc
FLAGS = -Wall -Wextra -MMD -MP -g -fsanitize=address -fvisibility=hidden -rdynamic -fno-omit-frame-pointer
DIRS = lib/arch/x86_64 \
      lib/comps \
      lib/include \
      lib/internals \
      lib/utils \
      graves/comps \
      graves/core \
      graves/ent \
	  graves/slist \
	  graves/rlist \
	  graves/req \
	  graves/reg \
	  graves/defs \
	  graves/utils \
      api/ 

SRC_DIR = graves/
INC_DIRS = ${addprefix -I, ${DIRS}}
FLAGS += ${flags}

OUTPUT_DIR = build/
OUTPUT_DEPS= build/

FILES_TO_COMPILE = ${foreach _D, ${SRC_DIR},${wildcard ${_D}*.c}}
OUTPUT_FILES_NAME = ${patsubst %.c, ${OUTPUT_DIR}%.o, ${FILES_TO_COMPILE}}
DEPS=${patsubst %.c, ${OUTPUT_DEPS}%.d, ${FILES_TO_COMPILE}}

all: directories ${OUTPUT_FILES_NAME}
	make -C lib
	make -C entities
	${CC} ${FLAGS} ${OUTPUT_FILES_NAME} mvs.c -Lbuild -lmvs -Wl,-rpath,build ${INC_DIRS} -o ${OUTPUT_DIR}mvs

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
