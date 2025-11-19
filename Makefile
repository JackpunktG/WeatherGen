CC = gcc
MINGW_CC = x86_64-w64-mingw32-gcc
CFLAGS = -Wall -Wextra -g -O0
LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lm

# Include paths
INCLUDES = -Ilib/arena_memory -Ilib/SDL2 -Isrc/objects

# Windows include and lib paths
WIN_INCLUDES = -Ilib/arena_memory -Ilib/SDL2 -Isrc/objects -I./mingw-libs/include
WIN_LIBPATH = -L./mingw-libs/lib

# Source files
SRCS = src/main.c \
       src/objects/floatingtext.c \
       src/objects/weathergen.c \
       lib/arena_memory/arena_memory.c \
       lib/SDL2/SDL2lib.c

# Object files
OBJS = $(SRCS:.c=.o)

# Windows object files (in a separate win-objs directory)
WIN_OBJS = win-objs/main.o \
           win-objs/floatingtext.o \
           win-objs/weathergen.o \
           win-objs/arena_memory.o \
           win-objs/SDL2lib.o

# Executable name
TARGET ?= WeatherGen_Demo

# Default target - dynamic linking for development
all: $(TARGET)

# Regular dynamic build (for development)
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Create a default icon if none exists
create-icon:
	@if [ ! -f weathergen_icon.png ]; then \
		echo "Creating default icon..."; \
		if command -v convert >/dev/null 2>&1; then \
			convert -size 128x128 gradient:'#87CEEB-#4169E1' -pointsize 18 -fill white -gravity center -stroke black -strokewidth 1 -annotate +0+0 "Weather\nGen\nDemo" weathergen_icon.png; \
			echo "Default icon created: weathergen_icon.png"; \
		else \
			echo "ImageMagick not found. Please install it or create weathergen_icon.png manually."; \
		fi \
	else \
		echo "Using existing icon: weathergen_icon.png"; \
	fi

# Portable AppImage build with custom icon
linux: $(TARGET) create-icon
	@echo "Creating portable AppImage..."
	@mkdir -p AppDir/usr/bin AppDir/usr/share/applications
	@cp $(TARGET) AppDir/usr/bin/
	@echo "[Desktop Entry]" > AppDir/usr/share/applications/WeatherGen.desktop
	@echo "Type=Application" >> AppDir/usr/share/applications/WeatherGen.desktop
	@echo "Name=WeatherGen Demo" >> AppDir/usr/share/applications/WeatherGen.desktop
	@echo "Comment=Weather Effects Generator" >> AppDir/usr/share/applications/WeatherGen.desktop
	@echo "Exec=WeatherGen_Demo" >> AppDir/usr/share/applications/WeatherGen.desktop
	@echo "Categories=Game;Simulation;" >> AppDir/usr/share/applications/WeatherGen.desktop
	@if [ -f weathergen_icon.png ]; then \
		echo "Icon=weathergen_icon" >> AppDir/usr/share/applications/WeatherGen.desktop; \
		cp weathergen_icon.png AppDir/; \
	fi
	@if [ ! -f linuxdeploy-x86_64.AppImage ]; then \
		echo "Downloading linuxdeploy..."; \
		wget -q https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage; \
		chmod +x linuxdeploy-x86_64.AppImage; \
	fi
	@echo "Building AppImage..."
	@if [ -f AppDir/weathergen_icon.png ]; then \
		./linuxdeploy-x86_64.AppImage --appdir AppDir --executable AppDir/usr/bin/$(TARGET) --desktop-file AppDir/usr/share/applications/WeatherGen.desktop --icon-file AppDir/weathergen_icon.png --output appimage; \
	else \
		./linuxdeploy-x86_64.AppImage --appdir AppDir --executable AppDir/usr/bin/$(TARGET) --desktop-file AppDir/usr/share/applications/WeatherGen.desktop --output appimage; \
	fi
	@echo ""
	@echo "🎉 AppImage created successfully!"
	@ls -la *.AppImage 2>/dev/null
	@echo "This single file will run on any Linux distribution!"

# Windows build with proper DLL bundling
windows: win-objs $(WIN_OBJS)
	@if [ ! -d "mingw-libs/include" ]; then \
		echo "Error: mingw-libs not found. Run 'make setup-mingw' first and download SDL2 libraries."; \
		exit 1; \
	fi
	@echo "Building Windows executable..."
	$(MINGW_CC) $(WIN_OBJS) -o $(TARGET)_windows.exe \
		-static-libgcc -static-libstdc++ \
		$(WIN_LIBPATH) \
		-Wl,--subsystem,console \
		-lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer \
		-lm -luser32 -lgdi32 -lwinmm -limm32 \
		-lole32 -loleaut32 -lshell32 -lversion -luuid -lws2_32
	@echo "Creating complete Windows distribution package..."
	@rm -rf windows-dist
	@mkdir -p windows-dist
	@cp $(TARGET)_windows.exe windows-dist/
	@cp mingw-libs/bin/*.dll windows-dist/
	@if [ -d "src/assets" ]; then cp -r src/assets windows-dist/; fi
	@if [ -d "images" ]; then cp -r images windows-dist/; fi
	@echo "# WeatherGen Demo - Windows Distribution" > windows-dist/README.txt
	@echo "" >> windows-dist/README.txt
	@echo "To run the weather generator demo:" >> windows-dist/README.txt
	@echo "1. Double-click WeatherGen_Demo_windows.exe" >> windows-dist/README.txt
	@echo "2. Or run from command line: WeatherGen_Demo_windows.exe" >> windows-dist/README.txt
	@echo "" >> windows-dist/README.txt
	@echo "All required DLLs are included in this folder." >> windows-dist/README.txt
	@echo "No additional software installation required!" >> windows-dist/README.txt
	@echo "" >> windows-dist/README.txt
	@date >> windows-dist/README.txt
	@echo ""
	@echo "🎉 Complete Windows distribution created!"
	@echo "📁 Location: windows-dist/"
	@echo "📄 Contents:"
	@ls -la windows-dist/
	@echo ""
	@echo "✅ Ready to distribute: Zip the 'windows-dist' folder"
	@echo "🎯 Users can extract and run without installing anything!"

# Create release packages for distribution
release: appimage windows
	@echo "Creating release packages..."
	@mkdir -p releases
	@cp *.AppImage releases/ 2>/dev/null || true
	@if [ -d windows-dist ]; then \
		cd windows-dist && zip -r ../releases/WeatherGen_Demo_Windows_$(shell date +%Y%m%d).zip * && cd ..; \
	fi
	@echo ""
	@echo "🎉 Release packages created in releases/ directory:"
	@ls -la releases/
	@echo ""
	@echo "Ready for distribution! 🚀"

# Create win-objs directory
win-objs:
	@mkdir -p win-objs

# Compile source files (regular builds)
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Windows object files
win-objs/main.o: src/main.c | win-objs
	$(MINGW_CC) $(CFLAGS) $(WIN_INCLUDES) -c $< -o $@

win-objs/floatingtext.o: src/objects/floatingtext.c | win-objs
	$(MINGW_CC) $(CFLAGS) $(WIN_INCLUDES) -c $< -o $@

win-objs/weathergen.o: src/objects/weathergen.c | win-objs
	$(MINGW_CC) $(CFLAGS) $(WIN_INCLUDES) -c $< -o $@

win-objs/arena_memory.o: lib/arena_memory/arena_memory.c | win-objs
	$(MINGW_CC) $(CFLAGS) $(WIN_INCLUDES) -c $< -o $@

win-objs/SDL2lib.o: lib/SDL2/SDL2lib.c | win-objs
	$(MINGW_CC) $(CFLAGS) $(WIN_INCLUDES) -c $< -o $@

# Check Windows executable dependencies
check-windows:
	@echo "Checking for Windows executables..."
	@if [ -f $(TARGET)_windows.exe ]; then \
		echo "Windows executable found. Checking dependencies:"; \
		echo "DLL dependencies:"; \
		x86_64-w64-mingw32-objdump -p $(TARGET)_windows.exe | grep "DLL Name" | head -10; \
		echo ""; \
		echo "✅ All SDL2 DLLs should be bundled in windows-dist/"; \
		echo "✅ Windows system DLLs (KERNEL32, msvcrt, SHELL32) are built into Windows"; \
	else \
		echo "No Windows executable found. Run 'make windows' first."; \
	fi

# Clean only build artifacts (preserves executables)
clean:
	rm -f $(OBJS)
	rm -rf win-objs AppDir

# Clean everything including executables and tools
clean-all:
	rm -f $(OBJS) $(TARGET) $(TARGET)_windows*.exe
	rm -rf win-objs AppDir *.AppImage mingw-libs linuxdeploy-x86_64.AppImage windows-dist releases

.PHONY: all appimage windows release clean clean-all check-windows win-objs create-icon
