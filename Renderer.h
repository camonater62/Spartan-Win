#pragma once

#include "IndexBuffer.h"
#include "Macros.h"
#include "Shader.h"
#include "VertexArray.h"

#include <GL/glew.h>
#include <signal.h>

void GLClearError();

bool GLLogCall(const char *function, const char *file, int line);

class Renderer {
public:
    void Clear() const;
    void Draw(const VertexArray &va, const IndexBuffer &ib, const Shader &shader) const;
};