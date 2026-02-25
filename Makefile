BUILD_DIR := bin
GENERATOR := Ninja
VERSION := 4.2.1

.PHONY: build build-clean clean configure

build: $(BUILD_DIR)/build.ninja
	cmake --build $(BUILD_DIR)

# Configure only if not already configured
$(BUILD_DIR)/build.ninja:
	cmake -S . -B $(BUILD_DIR) -G $(GENERATOR) -DCMAKE_POLICY_VERSION_MINIMUM=$(VERSION)

# Remove build directory
clean:
	rm -rf $(BUILD_DIR)

run:
	$(BUILD_DIR)/SIGSEGV

# Clean and rebuild from scratch
build-clean: clean build