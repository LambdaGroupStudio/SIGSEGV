BUILD_DIR := bin
GENERATOR := Ninja

.PHONY: build build-clean clean configure

build: $(BUILD_DIR)/build.ninja
	cmake --build $(BUILD_DIR)

# Configure only if not already configured
$(BUILD_DIR)/build.ninja:
	cmake -S . -B $(BUILD_DIR) -G $(GENERATOR)

# Remove build directory
clean:
	rm -rf $(BUILD_DIR)

# Clean and rebuild from scratch
build-clean: clean build