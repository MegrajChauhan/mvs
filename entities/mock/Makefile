CC = gcc
FLAGS = -Wall -Wextra -MMD -MP -g -fsanitize=address -fno-omit-frame-pointer -fPIC
DIRS = ${INC_DIRS} 
SRC_DIR = ./
INC_DIRS = ${addprefix -I, ${DIRS}}
OUTPUT_DIR = ${BUILD_DIR}
OUTPUT_DEPS= ${DEP_DIR}

FILES_TO_COMPILE = ${foreach _D, ${SRC_DIR},${wildcard ${_D}*.c}}
OUTPUT_FILES_NAME = ${patsubst %.c, ${OUTPUT_DIR}%.o, ${FILES_TO_COMPILE}}
DEPS=${patsubst %.c, ${OUTPUT_DEPS}%.d, ${FILES_TO_COMPILE}}

all: directories ${OUTPUT_FILES_NAME}
	${CC} -shared -o ${OBJ_DIR}/mock.so ${OUTPUT_FILES_NAME} -L${LINK_DIR} -Wl,-rpath,${LINK_DIR} ${LINK_OBJS}

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
