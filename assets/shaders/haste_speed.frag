// Haste/Speed - Classic RPG speed boost effect (FF series)
#version 120

uniform sampler2D texture;
uniform float time;
uniform float speedLevel; // 0.0 to 1.0 - intensity of speed effect

void main()
{
    vec2 texCoord = gl_TexCoord[0].xy;
    
    // Motion blur effect
    vec4 texColor = texture2D(texture, texCoord);
    
    // Sample multiple points for motion blur
    vec4 blurColor = texColor;
    float blurAmount = speedLevel * 0.02;
    
    for (int i = 1; i <= 3; i++) {
        float offset = float(i) * blurAmount;
        blurColor += texture2D(texture, texCoord + vec2(offset, 0.0));
        blurColor += texture2D(texture, texCoord - vec2(offset, 0.0));
    }
    blurColor /= 7.0; // Average the samples
    
    // Energy trails effect
    float trail = sin(time * 8.0 + texCoord.x * 20.0) * 0.3 + 0.7;
    
    // Yellow/orange speed aura
    vec3 speedColor = vec3(1.0, 0.8, 0.2);
    vec3 aura = speedColor * speedLevel * trail * 0.4;
    
    // Combine effects
    vec3 finalColor = blurColor.rgb + aura;
    
    // Add slight brightness boost
    finalColor += vec3(0.1) * speedLevel;
    
    gl_FragColor = vec4(finalColor, texColor.a) * gl_Color;
}
