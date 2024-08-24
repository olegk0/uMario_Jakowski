#include "gl_subs.h"
#include "Shaders.h"

#if 0
#define rmask 0xff000000
#define gmask 0x00ff0000
#define bmask 0x0000ff00
#define amask 0x000000ff
#else
#define rmask 0x000000ff
#define gmask 0x0000ff00
#define bmask 0x00ff0000
#define amask 0xff000000
#endif

typedef union {
    float gl[8];
    struct {
        float left_bottom_x;
        float left_bottom_y;
        float right_bottom_x;
        float right_bottom_y;
        float left_top_x;
        float left_top_y;
        float right_top_x;
        float right_top_y;
    };
} square_coord_t;

typedef union {
    float gl[4];
    struct {
        float r;
        float g;
        float b;
        float a;
    };
} gl_color_t;

static square_coord_t def_text;
static GLint a_position_location_tex;
static GLint a_texture_coordinates_location;
static GLint u_texture_unit_location;
static GLint transf_location_tex;

static GLint a_position_location_drw;
static GLint a_color_dwr;
static GLint transf_location_drw;

static GLfloat vscaleMatrix[4 * 4];
static GLuint shader_program_tex_2d;
static GLuint shader_program_drw_2d;
static SDL_GLContext context;
SDL_Window* MainWindow;

static int disp_width = 800;
static int disp_height = 600;

static float disp_width_scale;
static float disp_height_scale;

static GLuint loadShader(const char* shader_src, GLenum shader_type) {
    GLint lret;
    GLuint shader_id;

    const char* ShaderSrc = shader_src;

    shader_id = glCreateShader(shader_type);
    if (shader_id == 0) {
        printf("glCreateShader failed: 0x%08X\n", glGetError());
        return 0;
    }

    glShaderSource(shader_id, 1, &ShaderSrc, NULL);
    glCompileShader(shader_id);

    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &lret);
    if (!lret) {
        char* log;

        printf("vertex shader compilation failed!\n");
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &lret);

        if (lret > 1) {
            // log = new char[lret + 1];
            // glGetShaderInfoLog(shader_id, lret, NULL, log);
            // printf("%s\n", log);
            // delete[] log;
            SDL_Log("Error load shaders, ret:%d\n", lret);
        }
        return 0;
    }

    return shader_id;
}

static GLuint createShadersPrg(const char* vertex_shader_src, const char* fragment_shader_src) {
    GLint lret;

    GLuint vertex_2d_shader = loadShader(vertex_shader_src, GL_VERTEX_SHADER);
    if (!vertex_2d_shader)
        return 0;
    GLuint fragment_2d_shader = loadShader(fragment_shader_src, GL_FRAGMENT_SHADER);
    if (!fragment_2d_shader)
        return 0;

    GLuint program = glCreateProgram();
    if (!program) {
        printf("failed to create program!\n");
        return 0;
    }

    glAttachShader(program, vertex_2d_shader);
    glAttachShader(program, fragment_2d_shader);

    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &lret);
    if (!lret) {
        char* log;

        printf("program linking failed!\n");
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &lret);

        if (lret > 1) {
            // log = new char[lret + 1];
            // glGetProgramInfoLog(program, lret, NULL, log);
            // printf("%s\n", log);
            // delete[] log;
            SDL_Log("Error make shaders, ret:%d\n", lret);
        }
        return 0;
    }
    return program;
}

static int shadersPrepare() {
    shader_program_tex_2d = createShadersPrg(vertex_2d_shader_source1, fragment_2d_shader_source1);
    if (shader_program_tex_2d <= 0) {
        SDL_Log("Error createShadersPrg for tex\n");
        return -1;
    }
    a_position_location_tex = glGetAttribLocation(shader_program_tex_2d, "a_Position");
    a_texture_coordinates_location = glGetAttribLocation(shader_program_tex_2d, "a_TextureCoordinates");
    u_texture_unit_location = glGetUniformLocation(shader_program_tex_2d, "u_TextureUnit");
    transf_location_tex = glGetUniformLocation(shader_program_tex_2d, "u_transformation");

    shader_program_drw_2d = createShadersPrg(vertex_2d_shader_source2, fragment_2d_shader_source2);
    if (shader_program_drw_2d <= 0) {
        SDL_Log("Error createShadersPrg for drw\n");
        return -1;
    }
    a_position_location_drw = glGetAttribLocation(shader_program_drw_2d, "a_Position");
    a_color_dwr = glGetAttribLocation(shader_program_drw_2d, "a_Color");
    transf_location_drw = glGetUniformLocation(shader_program_drw_2d, "u_transformation");

    return 0;
}

static void setTextureMatrix(square_coord_t* Matrix, float x0, float y0, float x1, float y1) {
    Matrix->left_bottom_x = x0;
    Matrix->left_bottom_y = y1;
    Matrix->left_top_x = x0;
    Matrix->left_top_y = y0;
    Matrix->right_top_x = x1;
    Matrix->right_top_y = y0;
    Matrix->right_bottom_x = x1;
    Matrix->right_bottom_y = y1;
}

static void calcGlCoord(square_coord_t* Matrix, SDL_Rect* rect) {
    float f;

    f = (float)rect->w * disp_width_scale;
    Matrix->right_top_x = f;
    Matrix->right_bottom_x = f;

    f = (float)rect->h * disp_height_scale;
    Matrix->right_top_y = f;
    Matrix->left_top_y = f;

    f = (float)rect->x * disp_width_scale - (disp_width >> 1);
    Matrix->left_bottom_x = f;
    Matrix->left_top_x = f;
    Matrix->right_top_x += f;
    Matrix->right_bottom_x += f;

    f = (float)(448.0f - rect->h - rect->y) * disp_height_scale - (disp_height >> 1);
    Matrix->right_bottom_y = f;
    Matrix->left_bottom_y = f;
    Matrix->left_top_y += f;
    Matrix->right_top_y += f;
}

void blitTexture(GLuint texture, SDL_Rect* rect, square_coord_t* ctextr) {
    square_coord_t dpos_coord;

    calcGlCoord(&dpos_coord, rect);

    glBindTexture(GL_TEXTURE_2D, texture);
    glUseProgram(shader_program_tex_2d);
    glEnableVertexAttribArray(a_position_location_tex);
    glEnableVertexAttribArray(a_texture_coordinates_location);
    glVertexAttribPointer(a_position_location_tex, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), dpos_coord.gl);
    glVertexAttribPointer(a_texture_coordinates_location, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), ctextr->gl);
    glUniformMatrix4fv(transf_location_tex, 1, GL_FALSE, vscaleMatrix);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableVertexAttribArray(0);
}

inline void set_v_cplor(gl_color_t& to, gl_color_t& from) {
    to.r = from.r;
    to.g = from.g;
    to.b = from.b;
    to.a = from.a;
}

static void blitSquare(SDL_Rect* rect, gl_color_t& color) {
    square_coord_t dpos_coord;
    gl_color_t v_color[4];

    set_v_cplor(v_color[0], color);
    set_v_cplor(v_color[1], color);
    set_v_cplor(v_color[2], color);
    set_v_cplor(v_color[3], color);

    calcGlCoord(&dpos_coord, rect);
    //    glUniform1i(u_texture_unit_location, 0);

    glUseProgram(shader_program_drw_2d);
    glEnableVertexAttribArray(a_position_location_drw);
    glVertexAttribPointer(a_position_location_drw, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), dpos_coord.gl);
    glUniformMatrix4fv(transf_location_drw, 1, GL_FALSE, vscaleMatrix);
    glEnableVertexAttribArray(a_color_dwr);
    glVertexAttribPointer(a_color_dwr, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), v_color[0].gl);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableVertexAttribArray(0);
}

void BlitSquare(SDL_Rect* rect, float r, float g, float b, float a) {
    gl_color_t color;
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
    blitSquare(rect, color);
}

void BlitTexture(GLuint texture, SDL_Rect* rect) {
    square_coord_t ctextr;
    setTextureMatrix(&ctextr, 0, 0, 1, 1);
    blitTexture(texture, rect, &ctextr);
}

void BlitTextureFlip(GLuint texture, SDL_Rect* rect, bool flip_h, bool flip_v) {
    square_coord_t ctextr;

    float x0_cf = 0.0f;
    float y0_cf = 0.0f;
    float x1_cf = 1.0f;
    float y1_cf = 1.0f;
    if (flip_v) {
        x0_cf = -1.0f;
        x1_cf = 0.0f;
    }
    if (flip_h) {
        y0_cf = -1.0f;
        y1_cf = 0.0f;
    }
    setTextureMatrix(&ctextr, x0_cf, y0_cf, x1_cf, y1_cf);

    blitTexture(texture, rect, &ctextr);
}

void BlitTextureCrop(GLuint texture, SDL_Rect* dst_rect, SDL_Rect* src_rect, SDL_Rect* src_crop) {
    square_coord_t ctextr;
    float x0, y0, f;
    //
    f = (float)src_crop->w / (float)src_rect->w;
    x0 = (float)src_crop->x / (float)src_rect->w;
    x0 = x0 + f / 15.0f;
    y0 = (float)src_crop->y / (float)src_rect->h;
    setTextureMatrix(&ctextr, x0, y0, x0 + f, y0 + (float)src_crop->h / (float)src_rect->h);

    blitTexture(texture, dst_rect, &ctextr);
}

int GL_Init(const char* window_title, Uint32 init_flag) {
    int ret = SDL_Init(init_flag);
    if (ret) {
        SDL_Log("SDL_Init ret:%d\n", ret);
        return ret;
    }

    SDL_DisplayMode mode;
    SDL_GetDisplayMode(0, 0, &mode);
    disp_width = mode.w;
    disp_height = mode.h;
    SDL_Log("Screen  h:%d  w:%d", disp_height, disp_width);
    // disp_width = 800;
    // disp_height = 448;

    disp_width_scale = (float)disp_width / 800.0f;
    disp_height_scale = (float)disp_height / 448.0f;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    MainWindow = SDL_CreateWindow(window_title, 0, 0, disp_width, disp_height, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
    if (MainWindow) {
        float px_step_x = 2;
        px_step_x = px_step_x / disp_width;
        float px_step_y = 2;
        px_step_y = px_step_y / disp_height;

        for (int i = 0; i < 16; i++) {
            vscaleMatrix[i] = 0.0f;
        }

        vscaleMatrix[0] = px_step_x;
        vscaleMatrix[0 + 5] = px_step_y;
        vscaleMatrix[0 + 5 + 5] = 1.0f;
        vscaleMatrix[0 + 5 + 5 + 5] = 1.0f;

        context = SDL_GL_CreateContext(MainWindow);

        // SDL info
        SDL_version compiled;
        SDL_version linked;
        SDL_VERSION(&compiled);
        SDL_GetVersion(&linked);
        SDL_Log("SDL compiled version: %d.%d.%d\n", (int)compiled.major, (int)compiled.minor, (int)compiled.patch);
        SDL_Log("SDL linked version  : %d.%d.%d\n", (int)linked.major, (int)linked.minor, (int)linked.patch);

        // GL info
        SDL_Log("GL_VENDOR   : %s\n", glGetString(GL_VENDOR));
        SDL_Log("GL_RENDERER : %s\n", glGetString(GL_RENDERER));
        SDL_Log("GL_VERSION  : %s\n", glGetString(GL_VERSION));
        SDL_Log("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
        float pointSizeRange[2] = {-1.0, -1.0};
        glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, pointSizeRange);
        SDL_Log("gl_PointSize min: %d\n", pointSizeRange[0]);
        SDL_Log("gl_PointSize max: %d\n", pointSizeRange[1]);

        shadersPrepare();
        // CheckStatus(shader_program_2d, false);

        glViewport(0, 0, disp_width, disp_height);

        glEnable(GL_BLEND);

        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        // glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);

        // glClearColor(0.8, 1.0, 1.0, 1.0);
        glClearColor(0.0, 0.0, 0.0, 0.0);  // set background transparent

        // initTextureMatrix(&def_text, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        return 0;
    }
    return -1;
}

void GL_Free(void) {
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(MainWindow);
    SDL_Quit();
}

GLuint InitTexture(SDL_Surface* src_surf) {
    if (src_surf == NULL) {
        SDL_Log("Error empty surface!\n");
        return 0;
    }
    GLuint texture = 0;
    SDL_Surface* ts = SDL_CreateRGBSurface(0, src_surf->w, src_surf->h, 32, rmask, gmask, bmask, amask);
    if (ts) {
        SDL_BlitSurface(src_surf, NULL, ts, NULL);
        // SDL_Log("InitTexture. h:%d  w:%d  pitch:%d  BitsPerPixel:%d  format:%d\n", src_surf->h, src_surf->w, src_surf->pitch, src_surf->format->BitsPerPixel, src_surf->format->format);
        //  img->pixels
        //  img->h
        //  img->w

        // if (img->h < 2 || img->w < 2 || img->w != img->pitch) {
        //     SDL_Log("Invalid  h:%d  w:%d\n", img->h, img->w);
        //    return 0;
        // }

        glGenTextures(1, &texture);
        // SDL_Log("glGenTextures. ret:%d\n", glGetError());

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ts->w, ts->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, ts->pixels);

#if 1
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);      // GL_NEAREST GL_LINEAR_MIPMAP_NEAREST
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);       // GL_NEAREST GL_LINEAR
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);  // GL_CLAMP_TO_EDGE, GL_MIRRORED_REPEAT, or GL_REPEAT
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
#else
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  // GL_NEAREST GL_LINEAR_MIPMAP_NEAREST
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  // GL_NEAREST GL_LINEAR
#endif
        //  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img->w, img->h, GL_RGB, GL_UNSIGNED_BYTE, img->pixels);

        glBindTexture(GL_TEXTURE_2D, 0);

        SDL_FreeSurface(ts);
    }
    /*  GLuint framebuffer;
glGenFramebuffers(1, &framebuffer);
glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)framebuffer);
// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_map, 0);
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
glBindFramebuffer(GL_FRAMEBUFFER, 0);
*/
    return texture;
}

void InitTextureAndRender(int width, int height, SDL_Surface** surface, SDL_Renderer** renderer) {
    *surface = SDL_CreateRGBSurface(0, width, height, 32, rmask, gmask, bmask, amask);
    *renderer = SDL_CreateSoftwareRenderer(*surface);
}

void UpdateTexture(GLuint texture, SDL_Surface* img) {
    // img->pixels
    // img->h
    // img->w
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img->w, img->h, GL_RGBA, GL_UNSIGNED_BYTE, img->pixels);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void BlitScreen(GLuint texture, SDL_Surface* img) {
    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    // rect.h = img->h;
    // rect.w = img->w;
    rect.h = disp_height;
    rect.w = disp_width;
    UpdateTexture(texture, img);
    // BlitTexture(texture, &rect);
    SDL_GL_SwapWindow(MainWindow);
}