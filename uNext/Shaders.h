/*
 * Shaders.h
 *
 *  Created on: 10 апр. 2018 г.
 *      Author: olegvedi@gmail.com
 */

#ifndef SHADERS_H_
#define SHADERS_H_

static const char *vertex_2d_shader_source1 =
    "#version 100   \n"
    "attribute vec4 a_Position;   \n"
    "attribute vec2 a_TextureCoordinates;   \n"
    "varying vec2 v_TextureCoordinates;   \n"
    "uniform mat4 u_transformation; \n"
    "   \n"
    "void main()   \n"
    "{   \n"
    "    v_TextureCoordinates = a_TextureCoordinates;   \n"
    "    gl_Position = a_Position * u_transformation;   \n"
    "} \n";

/*    const char *vertex_2d_shader_source = "attribute vec4 a_Position;   \n"
 "attribute vec2 a_TextureCoordinates;   \n"
 "varying vec2 v_TextureCoordinates;   \n"
 "   \n"
 "void main()   \n"
 "{   \n"
 "    v_TextureCoordinates = a_TextureCoordinates;   \n"
 "    gl_Position = a_Position;   \n"
 "} \n";
 */
static const char *fragment_2d_shader_source1 =
    "#version 100   \n"
    "precision mediump float;   \n"
    "   \n"
    "uniform sampler2D u_TextureUnit;   \n"
    "varying vec2 v_TextureCoordinates;   \n"
    "   \n"
    "void main()   \n"
    "{   \n"
    "    gl_FragColor = texture2D(u_TextureUnit, v_TextureCoordinates);   \n"
    "} \n";
//////////////////////
static const char *vertex_2d_shader_source2 =
    "#version 100   \n"
    "attribute vec4 a_Position;   \n"
    "attribute vec4 a_Color;   \n"
    "varying vec4 u_Color;   \n"
    "uniform mat4 u_transformation; \n"
    "   \n"
    "void main()   \n"
    "{   \n"
    "    u_Color = a_Color;   \n"
    "    gl_Position = a_Position * u_transformation;   \n"
    "} \n";

static const char *fragment_2d_shader_source2 =
    "#version 100   \n"
    "precision mediump float;   \n"
    "   \n"
    "varying vec4 u_Color;   \n"
    "   \n"
    "void main()   \n"
    "{   \n"
    "    gl_FragColor = u_Color;   \n"
    "} \n";

#endif /* SHADERS_H_ */
