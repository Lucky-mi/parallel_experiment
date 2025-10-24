CC=gcc

CFLAGS=-I./inc -pthread -O3 -Wall
LDFLAGS=-lopenblas -lm -pthread
SRCDIR=./src
OBJDIR=./obj
BINDIR=./bin
TARGET=$(BINDIR)/gemm_main
SRCS=$(wildcard $(SRCDIR)/*.c)
OBJS=$(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))
all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete: $(BINDIR)/$@"
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled: $< -> $@"
.PHONY: clean
clean:
	rm -rf $(OBJDIR)/*.o $(TARGET)
	@echo "Clean complete."	
