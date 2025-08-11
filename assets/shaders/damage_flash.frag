// Damage Flash - Classic RPG hit effect (FF1-6 style)
#version 120

uniform sampler2D texture;
uniform float flashIntensity; // 0.0 to 1.0
uniform vec3 flashColor;      // Usually white or red

void main()
{
    vec4 texColor = texture2D(texture, gl_TexCoord[0].xy);
    
    // Flash effect - blend with flash color based on intensity
    vec3 finalColor = mix(texColor.rgb, flashColor, flashIntensity);
    
    gl_FragColor = vec4(finalColor, texColor.a) * gl_Color;
}
