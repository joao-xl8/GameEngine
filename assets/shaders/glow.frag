// Enhanced glow effect fragment shader
#version 120

uniform sampler2D texture;
uniform float time;
uniform vec4 glowColor;
uniform float glowIntensity;

void main()
{
    vec2 texCoord = gl_TexCoord[0].xy;
    vec4 texColor = texture2D(texture, texCoord);
    
    // Create a pulsing glow effect
    float pulse = sin(time * 3.0) * 0.5 + 0.5; // Oscillates between 0 and 1
    
    // Create edge detection for glow
    vec2 texelSize = vec2(1.0 / 256.0); // Approximate texel size
    
    // Sample neighboring pixels
    vec4 up = texture2D(texture, texCoord + vec2(0.0, texelSize.y));
    vec4 down = texture2D(texture, texCoord - vec2(0.0, texelSize.y));
    vec4 left = texture2D(texture, texCoord - vec2(texelSize.x, 0.0));
    vec4 right = texture2D(texture, texCoord + vec2(texelSize.x, 0.0));
    
    // Calculate edge strength
    float edge = length((up - down) + (left - right));
    
    // Combine original color with glow
    vec4 glow = glowColor * glowIntensity * pulse * (1.0 + edge * 2.0);
    
    // Blend the glow with the original texture
    gl_FragColor = texColor + glow * 0.5;
    gl_FragColor.a = texColor.a; // Preserve original alpha
    
    gl_FragColor *= gl_Color;
}
