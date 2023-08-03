#pragma once

#include <GL/glew.h>
#include <signal.h>

#ifdef _WIN32
#define ASSERT(x)                                                                                  \
	if (!(x))                                                                                      \
		__debugbreak();
#else
#define ASSERT(x)                                                                                  \
    if (!(x))                                                                                      \
        raise(SIGTRAP);
#endif

#ifdef _DEBUG
#define GLCall(x)                                                                                  \
    GLClearError();                                                                                \
    x;                                                                                             \
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#else
#define GLCall(x) x
#endif