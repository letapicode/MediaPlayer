#version 120
varying vec2 vTex;
uniform sampler2D texY;
uniform sampler2D texU;
uniform sampler2D texV;
void main() {
    float y = texture2D(texY, vTex).r;
    float u = texture2D(texU, vTex).r - 0.5;
    float v = texture2D(texV, vTex).r - 0.5;
    vec3 rgb;
    rgb.r = y + 1.402 * v;
    rgb.g = y - 0.344136 * u - 0.714136 * v;
    rgb.b = y + 1.772 * u;
    gl_FragColor = vec4(rgb, 1.0);
}
