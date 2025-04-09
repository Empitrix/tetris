OUTPUT := output

ifeq ($(OS), Windows_NT)
default: windows
else
default: linux
endif

# compile for linux
linux:
	@ cc ./main.c -lm -lraylib -DDEKSTOP_PLATFORM -o $(OUTPUT)

# compile for windows
windows:
	@ gcc main.c -o $(OUTPUT).exe -DDEKSTOP_PLATFORM -O1 -Wall -std=c99 -Wno-missing-braces -I support/win32/include/ -L support/win32/lib/ -lraylib -lopengl32 -lm -lgdi32 -lwinmm

# Run by the specified platform
run:
ifeq ($(OS), Windows_NT)
	@./$(OUTPUT).exe
else
	@./$(OUTPUT)
endif


# Compile and then run
total:
	@ $(MAKE) $(default) --no-print-directory
	@ $(MAKE) run --no-print-directory


# Create for web using emscripten
wasm:
	@ emcc -o index.html main.c \
	support/web/lib/libraylib.a -DPLATFORM_WEB -DSUPPORT_CUSTOM_FRAME_CONTROL -DWEB_PLATFORM \
	-I support/web/include \
	-s 'EXPORTED_FUNCTIONS=["_free","_malloc","_main"]' \
	-s EXPORTED_RUNTIME_METHODS=ccall \
	-s ASYNCIFY \
	-s FORCE_FILESYSTEM=1 \
	-s TOTAL_MEMORY=64MB \
	-s USE_GLFW=3


# Http server for testing in web
server:
	@ python -m http.server

clean:
ifeq ($(OS), Windows_NT)
	@ if exist $(OUTPUT) del /f $(OUTPUT)
	@ if exist $(OUTPUT).exe del /f $(OUTPUT).exe
	@ if exist $(OUTPUT).wasm del /f $(OUTPUT).wasm
	@ if exist $(OUTPUT).html del /f $(OUTPUT).html
	@ if exist $(OUTPUT).js del /f $(OUTPUT).js
else
	@ [ -e $(OUTPUT) ] && rm -f $(OUTPUT)
	@ [ -e $(OUTPUT).exe ] && rm -f $(OUTPUT).exe
	@ [ -e $(OUTPUT).wasm ] && rm -f $(OUTPUT).wasm
	@ [ -e $(OUTPUT).html ] && rm -f $(OUTPUT).html
	@ [ -e $(OUTPUT).js ] && rm -f $(OUTPUT).js
endif


clangd:
	@ python gen_clangd.py
