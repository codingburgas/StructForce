#include "logo.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#endif

unsigned int g_logoTexture     = 0;
int          g_logoW           = 0;
int          g_logoH           = 0;

unsigned int g_logoDarkTexture = 0;
int          g_logoDarkW       = 0;
int          g_logoDarkH       = 0;

static unsigned int uploadTexture(const char* path, int& outW, int& outH) {
    int channels;
    unsigned char* data = stbi_load(path, &outW, &outH, &channels, 4);
    if (!data) return 0;

    unsigned int tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, outW, outH, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    return tex;
}

void loadLogo(const char* lightPath, const char* darkPath) {
    g_logoTexture     = uploadTexture(lightPath, g_logoW, g_logoH);
    g_logoDarkTexture = uploadTexture(darkPath,  g_logoDarkW, g_logoDarkH);
}
