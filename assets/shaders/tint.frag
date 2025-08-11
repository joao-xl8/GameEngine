// Simple color tint fragment shader
#version 120

uniform sampler2D texture;
uniform vec4 tintColor;
uniform float tintStrength;

void main()
{
    // Sample the texture
    vec4 texColor = texture2D(texture, gl_TexCoord[0].xy);
    
    // Apply tint
    vec4 finalColor = mix(texColor, texColor * tintColor, tintStrength);
    
    // Preserve original alpha
    finalColor.a = texColor.a;
    
    gl_FragColor = finalColor * gl_Color;
}
