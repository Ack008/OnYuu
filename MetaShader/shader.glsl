uniform float u_time;
uniform vec3 u_colorA;
uniform vec3 u_colorB;

float pulse(vec2 uv, float t) {
    float a = sin(uv.x * 14.0 + t);
    float b = cos(uv.y * 9.0 - t * 0.7);
    return 0.5 + 0.5 * a * b;
}

vec3 pickColor(float p, vec3 c1, vec3 c2) {
    if (p > 0.65) {
        return c1;
    } else if (p < 0.35) {
        return c2;
    } else {
        return mix(c1, c2, p);
    }
}

vec3 shadeColor(vec3 base, vec3 n, vec3 l) {
    float ndl = clamp(dot(normalize(n), normalize(l)), 0.0, 1.0);
    return clamp(base * (0.25 + 0.75 * ndl), vec3(0.0), vec3(1.0));
}

void fragmentMain() {
    float p = pulse(V_UV, u_time);
    vec3 chosen = pickColor(p, u_colorA, u_colorB);
    vec3 lightDir = LIGHTS[0].position.xyz - V_WORLD_POS;
    vec3 lit = shadeColor(chosen * V_COLOR.xyz, V_NORMAL, lightDir);
    COLOR = vec4(lit, 1.0);
}

void vertexMain() {
    vec3 wobble = vec3(
        V_WORLD_POS.x,
        V_WORLD_POS.y + 0.06 * sin(u_time + V_WORLD_POS.x * 4.0),
        V_WORLD_POS.z
    );
    POSITION = CAMERA_PROJ * CAMERA_VIEW * vec4(wobble, 1.0);
}