#ifndef GL_INIT_H_
#define GL_INIT_H_

#include <SDL.h>
#include <SDL_opengles2.h>

extern SDL_Window* MainWindow;

int GL_Init(const char* window_title, Uint32 init_flag);
void GL_Free(void);
GLuint InitTexture(SDL_Surface* img);
void InitTextureAndRender(int width, int height, SDL_Surface** surface, SDL_Renderer** renderer);
void BlitTexture(GLuint texture, SDL_Rect* rect);
void BlitTextureFlip(GLuint texture, SDL_Rect* rect, bool flip_h, bool flip_v);
void BlitTextureCrop(GLuint texture, SDL_Rect* dst_rect, SDL_Rect* src_rect, SDL_Rect* src_crop);
void BlitScreen(GLuint texture, SDL_Surface* img);
void BlitSquare(SDL_Rect* rect, float r, float g, float b, float a);
void UpdateTexture(GLuint texture, SDL_Surface* img);

#define BlitSquareNorm(rect, r, g, b, a) BlitSquare(rect, (float)r / 255.0, (float)g / 255.0, (float)b / 255.0, (float)a / 255.0)
#endif