// Pixelate fragment shader
#version 120

uniform sampler2D texture;
uniform vec2 textureSize;
uniform float pixelSize;

void main()
{
    vec2 texCoord = gl_TexCoord[0].xy;
    
    // Calculate pixel grid
    vec2 pixelCoord = floor(texCoord * textureSize / pixelSize) * pixelSize / textureSize;
    
    // Sample the texture at the pixelated coordinate
    vec4 texColor = texture2D(texture, pixelCoord);
    
    gl_FragColor = texColor * gl_Color;
}
