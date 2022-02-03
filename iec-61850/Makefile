LIBIEC_HOME=/home/ken/Develop/libiec61850

SERVER_BINARY_NAME = server
SERVER_SOURCES = mms_server.c
SERVER_SOURCES += static_model.c

PROJECT_ICD_FILE = ancillary_service.xml

include $(LIBIEC_HOME)/make/target_system.mk
include $(LIBIEC_HOME)/make/stack_includes.mk

all:	$(SERVER_BINARY_NAME)

include $(LIBIEC_HOME)/make/common_targets.mk

LDLIBS += -lm -lmodbus

CFLAGS=-std=c99 `pkg-config --cflags libmodbus`
CP = cp

model:	$(PROJECT_ICD_FILE)
	java -jar $(LIBIEC_HOME)/tools/model_generator/genmodel.jar $(PROJECT_ICD_FILE)

mu:
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(MU_BINARY_NAME) mu.c	$(INCLUDES) $(LIB_NAME) $(LDLIBS)

$(SERVER_BINARY_NAME):	$(SERVER_SOURCES) $(LIB_NAME)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(SERVER_BINARY_NAME) $(SERVER_SOURCES) $(INCLUDES) $(LIB_NAME) $(LDLIBS)
	mkdir -p vmd-filestore
	$(CP) $(SERVER_BINARY_NAME) vmd-filestore/IEDSERVER.BIN


clean:
	rm -f $(SERVER_BINARY_NAME)
	rm -f vmd-filestore/IEDSERVER.BIN
