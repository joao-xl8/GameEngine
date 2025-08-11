// Stone/Petrify - Classic RPG petrification effect (FF series)
#version 120

uniform sampler2D texture;
uniform float petrifyLevel; // 0.0 to 1.0 - how petrified the character is
uniform float time;

void main()
{
    vec4 texColor = texture2D(texture, gl_TexCoord[0].xy);
    
    // Stone color (gray with slight brown tint)
    vec3 stoneColor = vec3(0.6, 0.55, 0.5);
    
    // Gradual petrification
    vec3 finalColor = mix(texColor.rgb, stoneColor, petrifyLevel);
    
    // Add stone texture effect
    vec2 texCoord = gl_TexCoord[0].xy;
    float noise = sin(texCoord.x * 50.0) * sin(texCoord.y * 50.0) * 0.1;
    finalColor += noise * petrifyLevel;
    
    // Reduce saturation as petrification increases
    float gray = dot(finalColor, vec3(0.299, 0.587, 0.114));
    finalColor = mix(finalColor, vec3(gray), petrifyLevel * 0.8);
    
    // Slight sparkle effect during transformation
    if (petrifyLevel > 0.1 && petrifyLevel < 0.9) {
        float sparkle = sin(time * 8.0 + texCoord.x * 20.0 + texCoord.y * 20.0);
        if (sparkle > 0.7) {
            finalColor += vec3(0.3) * (1.0 - petrifyLevel);
        }
    }
    
    gl_FragColor = vec4(finalColor, texColor.a) * gl_Color;
}
