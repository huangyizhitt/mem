CXX = g++
RM = rm -rf
AR = ar rcs
MKDIR = mkdir -p

PRJ_DIR = $(shell pwd)
SRC_DIR = $(PRJ_DIR)/src
INC_DIR = $(PRJ_DIR)/inc
LIB_DIR = $(PRJ_DIR)/lib
OBJ_DIR = $(PRJ_DIR)/obj

srcs = $(wildcard $(SRC_DIR)/*.cpp)
objs = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(notdir $(srcs)))
lib = mem
share_lib = $(LIB_DIR)/libmem.so
static_lib = $(LIB_DIR)/libmem.a
target_share_lib = mem_test_share_lib
target_static_lib = mem_test_static_lib

CXX_FLAGS = -O3 -std=gnu++11 -I$(INC_DIR)
SHARED_FLAGS = -fPIC -shared

all: $(OBJ_DIR) $(LIB_DIR) $(target_share_lib) $(target_static_lib)

$(OBJ_DIR):
	$(MKDIR) $@

$(LIB_DIR):
	$(MKDIR) $@

$(target_share_lib): main.cpp $(share_lib)
	$(CXX) $^ -o $@ $(CXX_FLAGS) -L$(LIB_DIR) -l$(lib)

$(target_static_lib): main.cpp $(static_lib)
	$(CXX) $^ -o $@ $(CXX_FLAGS) -static -L$(LIB_DIR) -l$(lib)

$(share_lib): $(objs)
	$(CXX) $(SHARED_FLAGS) $^ -o $@

$(static_lib): $(objs)
	$(AR) $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	
	$(CXX) -c $< -o $@ $(CXX_FLAGS) $(SHARED_FLAGS)

clean:
	$(RM) $(objs) $(LIB_DIR)/* $(target_share_lib) $(target_static_lib)

.PHONY: all clean