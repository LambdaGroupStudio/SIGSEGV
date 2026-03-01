BUILD_DIR   := bin
GENERATOR   := Ninja
CMAKE_MIN   := 4.2.1
SRC_DIRS    := src include
TARGET      := SIGSEGV
CLANG_FORMAT := clang-format

COLOR_BLUE  := \033[1;34m
COLOR_CYAN  := \033[1;36m
COLOR_GREEN := \033[1;32m
COLOR_RESET := \033[0m

C_FILES   := $(shell find $(SRC_DIRS) -type f -name "*.c")
HDR_FILES := $(shell find $(SRC_DIRS) -type f -name "*.h")

.PHONY: build configure clean build-clean run fmt

build: configure
	cmake --build $(BUILD_DIR)

configure:
	cmake -S . -B $(BUILD_DIR) -G $(GENERATOR) \
	      -DCMAKE_POLICY_VERSION_MINIMUM=$(CMAKE_MIN)

run: build
	@./$(BUILD_DIR)/$(TARGET)

fmt:
	@echo -e "$(COLOR_BLUE)> Running clang-format...$(COLOR_RESET)"
	@for file in $(C_FILES) $(HDR_FILES); do \
		echo -e "  $(COLOR_CYAN)fmt$(COLOR_RESET) $$file"; \
		$(CLANG_FORMAT) -i "$$file"; \
	done
	@echo -e "$(COLOR_GREEN)- Formatting complete$(COLOR_RESET)"

clean:
	rm -rf $(BUILD_DIR)

build-clean: clean build
