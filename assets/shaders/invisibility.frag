// Invisibility - Classic RPG invisibility spell effect (FF series)
#version 120

uniform sampler2D texture;
uniform float time;
uniform float invisLevel; // 0.0 (visible) to 1.0 (invisible)

void main()
{
    vec4 texColor = texture2D(texture, gl_TexCoord[0].xy);
    
    // Shimmering effect
    vec2 texCoord = gl_TexCoord[0].xy;
    float shimmer = sin(time * 3.0 + texCoord.x * 10.0) * sin(time * 2.0 + texCoord.y * 8.0);
    shimmer = shimmer * 0.1 + 0.9; // Normalize to 0.8-1.0 range
    
    // Distortion effect for partial invisibility
    vec2 distortion = vec2(
        sin(time * 4.0 + texCoord.y * 15.0) * 0.01,
        cos(time * 3.0 + texCoord.x * 12.0) * 0.01
    ) * invisLevel;
    
    vec4 distortedColor = texture2D(texture, texCoord + distortion);
    
    // Blend between normal and distorted
    vec4 finalColor = mix(texColor, distortedColor, invisLevel * 0.5);
    
    // Apply shimmer effect
    finalColor.rgb *= shimmer;
    
    // Reduce alpha based on invisibility level
    finalColor.a *= (1.0 - invisLevel * 0.8); // Never fully invisible for gameplay
    
    gl_FragColor = finalColor * gl_Color;
}
