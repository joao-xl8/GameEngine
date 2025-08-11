// Simple test wave shader - very obvious effect
#version 120

uniform sampler2D texture;
uniform float time;

void main()
{
    vec2 texCoord = gl_TexCoord[0].xy;
    
    // Very obvious wave distortion
    float wave = sin(texCoord.y * 20.0 + time * 5.0) * 0.2;
    texCoord.x += wave;
    
    // Keep coordinates in bounds
    if (texCoord.x < 0.0) texCoord.x = 0.0;
    if (texCoord.x > 1.0) texCoord.x = 1.0;
    
    vec4 texColor = texture2D(texture, texCoord);
    
    gl_FragColor = texColor * gl_Color;
}
