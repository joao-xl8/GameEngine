// Freeze/Ice - Classic RPG ice spell effect (FF series)
#version 120

uniform sampler2D texture;
uniform float time;
uniform float freezeLevel; // 0.0 to 1.0 - how frozen the target is

void main()
{
    vec4 texColor = texture2D(texture, gl_TexCoord[0].xy);
    vec2 texCoord = gl_TexCoord[0].xy;
    
    // Ice blue tint
    vec3 iceColor = vec3(0.7, 0.9, 1.0);
    
    // Apply ice tint
    vec3 frozenColor = mix(texColor.rgb, texColor.rgb * iceColor, freezeLevel);
    
    // Ice crystal pattern
    float crystals = sin(texCoord.x * 30.0) * sin(texCoord.y * 30.0);
    crystals += sin(texCoord.x * 45.0 + time) * sin(texCoord.y * 35.0 + time * 0.5);
    crystals = crystals * 0.1 + 0.9;
    
    // Apply crystal effect
    frozenColor *= crystals;
    
    // Frost sparkles
    float sparkle = sin(time * 4.0 + texCoord.x * 50.0) * sin(time * 3.0 + texCoord.y * 40.0);
    if (sparkle > 0.8 && freezeLevel > 0.3) {
        frozenColor += vec3(0.4, 0.6, 0.8) * (sparkle - 0.8) * 5.0;
    }
    
    // Reduce saturation for frozen look
    float gray = dot(frozenColor, vec3(0.299, 0.587, 0.114));
    frozenColor = mix(frozenColor, vec3(gray) * iceColor, freezeLevel * 0.6);
    
    gl_FragColor = vec4(frozenColor, texColor.a) * gl_Color;
}
