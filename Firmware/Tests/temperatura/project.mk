PROJECT_NAME = test_temperatura

INC_FILES += $(TESTCOMMON_PATH)unity_src
INC_FILES += $(LIBS_EXTERNAL_PATH)my_stdio/inc
INC_FILES += $(LIBS_EXTERNAL_PATH)general/inc
INC_FILES += $(BASE_PATH)conformity_tests/12_PIDAFOTA/producto_dummy
INC_FILES += $(TESTCOMMON_PATH)mocks
INC_FILES += $(MEMO_FW_PATH)libs_tr0manager/inc
INC_FILES += $(MEMO_FW_PATH)inc
INC_FILES += $(MEMO_FW_PATH)common/inc
INC_FILES += $(MEMO_FW_PATH)common
INC_FILES += $(MAKEFILE_DIR)inc
INC_FILES += $(LIBS_EXTERNAL_PATH)coding/inc

#-------------------------------------------------------------------
SRC_FILES += $(TESTCOMMON_PATH)unity_src/unity.c
SRC_FILES += $(MEMO_FW_PATH)libs_tr0manager/src/protocolo_ida.c
SRC_FILES += $(MEMO_FW_PATH)libs_tr0manager/src/protocolo_ida_abstraction_layer.c
SRC_FILES += $(LIBS_EXTERNAL_PATH)my_stdio/src/my_sprintf.c
SRC_FILES += $(LIBS_EXTERNAL_PATH)my_stdio/src/my_sscanf.c
SRC_FILES += $(LIBS_EXTERNAL_PATH)my_stdio/src/my_stdio_helpers.c
SRC_FILES += $(LIBS_EXTERNAL_PATH)general/src/rstring.c
SRC_FILES += $(LIBS_EXTERNAL_PATH)general/src/convert.c
SRC_FILES += $(LIBS_EXTERNAL_PATH)coding/src/checksum_calc.c
SRC_FILES += $(TESTCOMMON_PATH)mocks/eeprom_support_mock.c

SRC_FILES += $(MAKEFILE_DIR)src/main.c
SRC_FILES += $(MAKEFILE_DIR)src/tests.c


CFLAGS += -DOS_NOT_VERBOSE=1
