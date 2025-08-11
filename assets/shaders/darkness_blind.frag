// Darkness/Blind - Classic RPG darkness/blind status effect (FF series)
#version 120

uniform sampler2D texture;
uniform float time;
uniform float darknessLevel; // 0.0 to 1.0 - intensity of darkness effect

void main()
{
    vec4 texColor = texture2D(texture, gl_TexCoord[0].xy);
    vec2 texCoord = gl_TexCoord[0].xy;
    
    // Dark purple/black tint
    vec3 darkColor = vec3(0.2, 0.1, 0.3);
    
    // Swirling darkness effect
    float swirl = sin(time * 2.0 + texCoord.x * 8.0) * cos(time * 1.5 + texCoord.y * 6.0);
    swirl = swirl * 0.3 + 0.7;
    
    // Apply darkness tint
    vec3 darkenedColor = mix(texColor.rgb, darkColor, darknessLevel * 0.7);
    
    // Reduce overall brightness
    darkenedColor *= (1.0 - darknessLevel * 0.5) * swirl;
    
    // Add dark energy wisps
    float wisps = sin(time * 3.0 + texCoord.x * 12.0 + texCoord.y * 8.0);
    wisps += sin(time * 2.0 + texCoord.x * 6.0 - texCoord.y * 10.0);
    wisps = smoothstep(0.5, 1.0, wisps * 0.5 + 0.5);
    
    // Apply wisp effect
    darkenedColor = mix(darkenedColor, darkColor, wisps * darknessLevel * 0.3);
    
    // Vignette effect for blindness
    float vignette = 1.0 - length(texCoord - vec2(0.5)) * 2.0;
    vignette = smoothstep(0.0, 1.0, vignette);
    darkenedColor *= mix(1.0, vignette, darknessLevel * 0.8);
    
    gl_FragColor = vec4(darkenedColor, texColor.a) * gl_Color;
}
