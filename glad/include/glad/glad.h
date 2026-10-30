#ifndef GLAD_GLAD_H_
#define GLAD_GLAD_H_

// Compatibility wrapper for GLAD 2 header layout and GLAD 1 naming
#include <glad/gl.h>

typedef GLADloadfunc GLADloadproc;

#ifndef gladLoadGLLoader
#define gladLoadGLLoader gladLoadGL
#endif

#endif // GLAD_GLAD_H_
