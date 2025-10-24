# --- 项目和文件定义 ---
# 编译器
CC = gcc


INC_DIRS = -I./inc -I/usr/include/openblas/x86_64-linux-gnu

# 源文件和目标文件
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# 所有要编译的 .c 源文件列表
SRCS = $(wildcard $(SRC_DIR)/*.c)
# 对应生成的 .o 目标文件
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# 要生成的可执行文件名
# 假设您想生成 gemm_test 和 spmv_test 两个程序
EXECS = $(BIN_DIR)/gemm_test $(BIN_DIR)/spmv_test

# --- CBLAS/BLAS 库设置 ---
# 库文件搜索路径 (CBLAS/BLAS 库通常在 /usr/lib 或 /usr/lib/x86_64-linux-gnu 等目录下)
# 一般系统会自动找到，但明确指出可以避免问题
LIB_DIRS = 
# 链接的库：
# -lopenblas: 如果您安装的是 OpenBLAS
# -lblas -llapack: 如果您使用的是标准的 BLAS/LAPACK 库
LIBS = -lopenblas -lm 

# 编译和链接标志
CFLAGS = -Wall -O3 $(INC_DIRS)
LDFLAGS = $(LIB_DIRS) $(LIBS)

# --- 规则定义 ---

# 默认目标：创建 obj 和 bin 目录，然后编译所有可执行文件
all: $(OBJ_DIR) $(BIN_DIR) $(EXECS)

# 目标 1: 编译 GEMM 测试程序
$(BIN_DIR)/gemm_test: $(OBJ_DIR)/gemm_main.o $(OBJ_DIR)/gemm.o $(OBJ_DIR)/spmv.o
	$(CC) $(OBJ_DIR)/gemm_main.o $(OBJ_DIR)/gemm.o -o $@ $(LDFLAGS)

# 目标 2: 编译 SPMV 测试程序
$(BIN_DIR)/spmv_test: $(OBJ_DIR)/spmv_main.o $(OBJ_DIR)/spmv.o $(OBJ_DIR)/gemm.o
	$(CC) $(OBJ_DIR)/spmv_main.o $(OBJ_DIR)/spmv.o -o $@ $(LDFLAGS)

# 通用规则：将源文件编译成目标文件
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# 目录创建规则
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# 清理目标
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)