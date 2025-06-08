/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#version 320 es
precision highp float;  // Higher precision than mediump (optional)

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

out vec2 v_uv;

uniform mat4 u_modelViewProjection;  // Standard naming convention

void main() {
    gl_Position = u_modelViewProjection * vec4(position, 1.0);
    v_uv = uv;
}
