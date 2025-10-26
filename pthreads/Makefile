CC=gcc

CFLAGS=-I./inc -pthread -O3 -Wall
LDFLAGS=-lopenblas -lm -pthread

SRCDIR=./src
OBJDIR=./obj
BINDIR=./bin

# ----------------------------------------------------
# 更新：定义所有四个目标
TARGETS=$(BINDIR)/gemm_row $(BINDIR)/gemm_2d $(BINDIR)/spmv_static $(BINDIR)/spmv_dynamic
# ----------------------------------------------------

all: $(TARGETS)

# --- GEMM 规则 ---
$(BINDIR)/gemm_row: $(OBJDIR)/gemm_row.o
	@mkdir -p $(BINDIR)
	$(CC) $^ -o $@ $(LDFLAGS)
	@echo "Built: $@"

$(BINDIR)/gemm_2d: $(OBJDIR)/gemm_2d.o
	@mkdir -p $(BINDIR)
	$(CC) $^ -o $@ $(LDFLAGS)
	@echo "Built: $@"

# ----------------------------------------------------
# 新增：SpMV 规则
$(BINDIR)/spmv_static: $(OBJDIR)/spmv_static.o $(OBJDIR)/spmv_utils.o
	@mkdir -p $(BINDIR)
	$(CC) $^ -o $@ $(LDFLAGS)
	@echo "Built: $@"

$(BINDIR)/spmv_dynamic: $(OBJDIR)/spmv_dynamic.o $(OBJDIR)/spmv_utils.o
	@mkdir -p $(BINDIR)
	$(CC) $^ -o $@ $(LDFLAGS)
	@echo "Built: $@"
# ----------------------------------------------------

# 通用编译规则 (保持不变)
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled: $< -> $@"

.PHONY: clean
clean:
	rm -rf $(OBJDIR)/*.o $(TARGETS)
	@echo "Clean complete."