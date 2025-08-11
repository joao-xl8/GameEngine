// Poison/Status Effect - Classic RPG status ailment visual (FF series)
#version 120

uniform sampler2D texture;
uniform float time;
uniform vec3 poisonColor; // Usually green or purple

void main()
{
    vec4 texColor = texture2D(texture, gl_TexCoord[0].xy);
    
    // Pulsing poison effect
    float pulse = sin(time * 4.0) * 0.3 + 0.7; // Oscillates between 0.4 and 1.0
    
    // Tint with poison color
    vec3 poisonTint = mix(texColor.rgb, poisonColor, 0.4 * pulse);
    
    // Add slight transparency variation for "sick" look
    float alpha = texColor.a * (0.8 + 0.2 * pulse);
    
    gl_FragColor = vec4(poisonTint, alpha) * gl_Color;
}
