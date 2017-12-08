#PATH DEL MAKEFILE
MAKEFILE_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

#PATH A LA BASE DEL FIRMWARE
BASE_PATH = $(abspath $(MAKEFILE_DIR)../../../)/

#-------------------------------------------------------------------
#PATH A LA BASE DE LA RUTA A COMMON
TESTCOMMON_PATH = $(BASE_PATH)conformity_tests/common/

#PATH A LA BASE DE LAS LIBS MODULES
LIBS_PATH = $(BASE_PATH)modules/

#PATH A LA BASE DE LAS LIBS SUPORT (LPC4337) Sacar o reaorganizar en funcion de que depende y que no del micro
LIBS_EXTERNAL_PATH = $(LIBS_PATH)lpc4337_m4/

#PATH A LA BASE DE PROYECTOS 
MEMO_FW_PATH = $(BASE_PATH)projects/

#-------------------------------------------------------------------
#PATH EN DONDE QUEDAN LOS ARCHIVOS OBJETO
OUT_PATH = $(MAKEFILE_DIR)out/

#includes all the project definitions
include project.mk

#flags que se le pasa al compilador
CFLAGS += $(foreach inc, $(INC_FILES), -I$(inc) )
CFLAGS += -D_WIN32 -ggdb

#regla que linkea luego de tener los requisitos
all: info objs
	@echo "*** Linkeando" $(PROJECT_NAME) "***"
	gcc $(OUT_PATH)*.o -o $(PROJECT_NAME)

#regla que compila cada .c por serparado en un .o
%.o: %.c
	@echo
	@echo "*** Compilando $< ***"
	@echo
	@mkdir -p $(OUT_PATH)
	@gcc -c $< -Wall $(CFLAGS) -o $(OUT_PATH)$(notdir $(patsubst %.c,%.o, $< ))

#regla que como prerequisito necesita tener todos los .o (que se compilan por separado).
objs: $(foreach src_file, $(SRC_FILES), $(patsubst %.c,%.o, $(src_file) ) )

#regla que imprime simplemente la informacion
info:
	@echo '---------------------------------------------------------------------'
	@echo 'Firmware Base Path:   '$(BASE_PATH)
	@echo 'Test Path:            '$(MAKEFILE_DIR)
	@echo 'Libs Path:            '$(LIBS_PATH)
	@echo 'Libs (external) Path: '$(LIBS_EXTERNAL_PATH)
	@echo '---------------------------------------------------------------------'
	@echo 'Main FW Path:         '$(MEMO_FW_PATH)
#	@echo 'MakeFile_List:        '$(MAKEFILE_LIST)
#	@echo 'MFL_Last_Word:        '$(lastword $(MAKEFILE_LIST))
#	@echo 'MFL_LW_AbsPath:       '$(abspath $(lastword $(MAKEFILE_LIST)))
	@echo '---------------------------------------------------------------------'

clean:
	@rm -rf $(OUT_PATH)*.o $(PROJECT_NAME)
	@rm -rf $(OUT_PATH)
