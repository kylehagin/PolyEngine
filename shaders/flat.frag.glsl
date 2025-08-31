#version 460
layout(location = 0) in vec3 vNrm;
layout(location = 0) out vec4 outColor;

void main()
{
    vec3 n = normalize(vNrm);
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    float ndl = max(dot(n, lightDir), 0.0);
    vec3 base = vec3(0.4, 0.7, 0.9);
    outColor = vec4(base * (0.2 + 0.8 * ndl), 1.0);
}
