CC = clang++
WARNINGS = -Wall -Wextra -Werror -Wpedantic -Wno-int-to-void-pointer-cast
CFLAGS = $(WARNINGS) -std=c++17 -march=x86-64-v2 -mtune=generic -pipe -flto=thin
NAME = Spartan
SRC = main.cpp \
		Camera.cpp \
		IndexBuffer.cpp \
		Plane.cpp \
		Renderer.cpp \
		Shader.cpp \
		Texture.cpp \
		VertexArray.cpp \
		VertexBuffer.cpp \
		Cube.cpp 
INCLUDE = -I.
LIBS = -lGL
DEFINE =

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	DEFINE += -D_DEBUG
	CFLAGS += -g -O1
	OBJ_DIR = debug
else
	DEFINE += -D_RELEASE -DNDEBUG
	CFLAGS += -O3 -g
	OBJ_DIR = release
endif
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))
OUT_EXE = $(NAME)

# Tests
SRC += Tests/Test.cpp \
		Tests/TestClearColor.cpp \
		Tests/TestTexture2D.cpp \
		Tests/TestCube.cpp \
		Tests/TestJolt.cpp \
		Tests/TestNoise.cpp \
		Tests/TestAssimp.cpp 
		
INCLUDE += -ITests

## Vendor
# STB
SRC += vendor/stb_image/stb_image.cpp
INCLUDE += -Ivendor/stb_image

# Dear ImGui
SRC += vendor/imgui/imgui.cpp \
		vendor/imgui/imgui_demo.cpp \
		vendor/imgui/imgui_draw.cpp \
		vendor/imgui/imgui_tables.cpp \
		vendor/imgui/imgui_widgets.cpp \
		vendor/imgui/backends/imgui_impl_glfw.cpp \
		vendor/imgui/backends/imgui_impl_opengl3.cpp
INCLUDE += -Ivendor/imgui -Ivendor/imgui/backends

# Assimp
LIBS += -Lvendor/assimp/lib -lassimp -lzlibstatic -lminizip
INCLUDE += -Ivendor/assimp/include

# GLFW
LIBS += -Lvendor/glfw -lglfw
INCLUDE += -Ivendor/glfw/include

# GLEW
LIBS += -Lvendor/glew/lib -lGLEW
INCLUDE += -Ivendor/glew/include

# GLM
INCLUDE += -Ivendor/glm

# Jolt
LIBS += -Lvendor/Jolt/lib -lJolt
INCLUDE += -Ivendor/Jolt/include
CFLAGS += -ffp-model=precise -pthread -mavx2 -mbmi -mpopcnt -mlzcnt -mf16c -mfma
DEFINE += -DJPH_DEBUG_RENDERER -DJPH_PROFILE_ENABLED -DJPH_USE_AVX -DJPH_USE_AVX2 -DJPH_USE_F16C -DJPH_USE_FMADD -DJPH_USE_LZCNT -DJPH_USE_SSE4_1 -DJPH_USE_SSE4_2 -DJPH_USE_TZCNT

default: $(OUT_EXE)

$(OUT_EXE): $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDE) $(DEFINE) -o $@ $^ $(LIBS)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDE) $(DEFINE) -c -o $@ $<

clean:
	rm -rf $(OUT_EXE) debug release
