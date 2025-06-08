/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#version 320 es
precision highp float;

in vec2 vuv;                // Changed from 'varying'
out vec4 fragColor;         // Replaces gl_FragColor

uniform sampler2D u_texture;  // Standardized naming convention
uniform vec4 u_baseColor;     // Added 'u_' prefix for uniforms

void main()
{
    fragColor = texture(u_texture, vuv) * u_baseColor;  // texture() replaces texture2D()
}
