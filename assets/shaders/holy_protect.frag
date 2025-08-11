// Holy/Protect - Classic RPG protection spell effect (FF series)
#version 120

uniform sampler2D texture;
uniform float time;
uniform float protectionLevel; // 0.0 to 1.0 - strength of protection

void main()
{
    vec4 texColor = texture2D(texture, gl_TexCoord[0].xy);
    vec2 texCoord = gl_TexCoord[0].xy;
    
    // Holy golden color
    vec3 holyColor = vec3(1.0, 0.9, 0.6);
    
    // Gentle pulsing
    float pulse = sin(time * 2.0) * 0.3 + 0.7;
    
    // Protective barrier effect - circular pattern
    float distance = length(texCoord - vec2(0.5));
    float barrier = sin(distance * 15.0 - time * 3.0) * 0.5 + 0.5;
    barrier = smoothstep(0.3, 0.7, barrier);
    
    // Holy light rays
    float rays = 0.0;
    for (int i = 0; i < 8; i++) {
        float angle = float(i) * 0.785398; // 45 degrees in radians
        vec2 rayDir = vec2(cos(angle), sin(angle));
        float rayIntensity = max(0.0, dot(normalize(texCoord - vec2(0.5)), rayDir));
        rays += pow(rayIntensity, 8.0);
    }
    rays *= 0.3;
    
    // Combine effects
    vec3 holyGlow = holyColor * (barrier * 0.4 + rays * 0.6) * protectionLevel * pulse;
    
    // Brighten the character slightly
    vec3 finalColor = texColor.rgb + holyGlow;
    finalColor += vec3(0.1) * protectionLevel * pulse;
    
    // Add slight golden tint
    finalColor = mix(finalColor, finalColor * holyColor, protectionLevel * 0.2);
    
    gl_FragColor = vec4(finalColor, texColor.a) * gl_Color;
}
