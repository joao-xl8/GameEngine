// Magic Charge - Classic RPG spell casting effect (FF4-6 style)
#version 120

uniform sampler2D texture;
uniform float time;
uniform float chargeLevel; // 0.0 to 1.0 - how charged the spell is
uniform vec3 magicColor;   // Blue, white, or spell-specific color

void main()
{
    vec4 texColor = texture2D(texture, gl_TexCoord[0].xy);
    
    // Pulsing magic energy
    float pulse = sin(time * 6.0) * 0.5 + 0.5;
    
    // Magic aura intensity based on charge level
    float intensity = chargeLevel * pulse;
    
    // Create magical glow
    vec3 magicGlow = magicColor * intensity * 0.8;
    
    // Blend original color with magic effect
    vec3 finalColor = texColor.rgb + magicGlow;
    
    // Add slight brightness increase when fully charged
    if (chargeLevel > 0.8) {
        finalColor += vec3(0.2) * pulse;
    }
    
    gl_FragColor = vec4(finalColor, texColor.a) * gl_Color;
}
