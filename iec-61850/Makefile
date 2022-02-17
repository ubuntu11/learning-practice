LIBIEC_HOME=/home/ken/Develop/libiec61850

SERVER_BINARY_NAME = server
SERVER_SOURCES = mms_server.c
SERVER_SOURCES += static_model.c

CLIENT_BINARY_NAME = client
CLIENT_SOURCES = mms_client.c

PROJECT_ICD_FILE = ancillary_service.xml

include $(LIBIEC_HOME)/make/target_system.mk
include $(LIBIEC_HOME)/make/stack_includes.mk

all:	$(CLIENT_BINARY_NAME)

include $(LIBIEC_HOME)/make/common_targets.mk

#LDLIBS += -lm -lmodbus
#CFLAGS=-std=c99 `pkg-config --cflags libmodbus`
CFLAGS=-std=c99
LDLIBS += -lm -lsqlite3
CP = cp

model:	$(PROJECT_ICD_FILE)
	java -jar $(LIBIEC_HOME)/tools/model_generator/genmodel.jar $(PROJECT_ICD_FILE)

$(SERVER_BINARY_NAME):	$(SERVER_SOURCES) $(LIB_NAME)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(SERVER_BINARY_NAME) $(SERVER_SOURCES) $(INCLUDES) $(LIB_NAME) $(LDLIBS)
	mkdir -p vmd-filestore
	$(CP) $(SERVER_BINARY_NAME) vmd-filestore/IEDSERVER.BIN

$(CLIENT_BINARY_NAME): $(SERVER_BINARY_NAME)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(CLIENT_BINARY_NAME) $(CLIENT_SOURCES) $(INCLUDES) $(LIB_NAME) $(LDLIBS)


clean:
	rm -f $(SERVER_BINARY_NAME) $(CLIENT_BINARY_NAME)
	rm -f static_model.*
	rm -rf vmd-filestore
