// Berserk/Rage - Classic RPG berserk status effect (FF series)
#version 120

uniform sampler2D texture;
uniform float time;
uniform float rageLevel; // 0.0 to 1.0 - intensity of rage

void main()
{
    vec4 texColor = texture2D(texture, gl_TexCoord[0].xy);
    vec2 texCoord = gl_TexCoord[0].xy;
    
    // Red rage aura
    vec3 rageColor = vec3(1.0, 0.2, 0.1);
    
    // Pulsing effect
    float pulse = sin(time * 6.0) * 0.4 + 0.6;
    
    // Screen shake simulation through slight distortion
    vec2 shake = vec2(
        sin(time * 20.0) * 0.005,
        cos(time * 25.0) * 0.005
    ) * rageLevel;
    
    vec4 shakenColor = texture2D(texture, texCoord + shake);
    
    // Red tint overlay
    vec3 rageTint = mix(shakenColor.rgb, rageColor, rageLevel * 0.3 * pulse);
    
    // Increase contrast and saturation
    rageTint = (rageTint - 0.5) * (1.0 + rageLevel * 0.5) + 0.5;
    rageTint = mix(vec3(dot(rageTint, vec3(0.299, 0.587, 0.114))), rageTint, 1.0 + rageLevel);
    
    // Add fiery edge effect
    float edge = length(texCoord - vec2(0.5));
    if (edge > 0.3) {
        float fireIntensity = (edge - 0.3) * 3.0 * rageLevel;
        rageTint += rageColor * fireIntensity * pulse;
    }
    
    gl_FragColor = vec4(rageTint, texColor.a) * gl_Color;
}
