/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#version 310 es
precision mediump float;

in vec2 vuv;
out vec4 fragColor;
uniform sampler2D texture;
uniform vec4 baseColor;

void main(void)
{
    fragColor = texture(texture, vuv) * baseColor;
}
