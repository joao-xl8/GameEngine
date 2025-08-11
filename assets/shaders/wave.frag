// Wave distortion fragment shader - enhanced visibility
#version 120

uniform sampler2D texture;
uniform float time;
uniform float amplitude;
uniform float frequency;

void main()
{
    vec2 texCoord = gl_TexCoord[0].xy;
    
    // Create horizontal wave distortion
    float wave = sin(texCoord.y * frequency + time * 2.0) * amplitude;
    texCoord.x += wave;
    
    // Also add a subtle vertical wave for more effect
    float wave2 = sin(texCoord.x * frequency * 0.5 + time * 1.5) * amplitude * 0.3;
    texCoord.y += wave2;
    
    // Wrap texture coordinates instead of clamping for continuous effect
    texCoord = fract(texCoord);
    
    // Sample the texture with distorted coordinates
    vec4 texColor = texture2D(texture, texCoord);
    
    gl_FragColor = texColor * gl_Color;
}
