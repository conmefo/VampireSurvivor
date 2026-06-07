uniform sampler2D texture;
uniform float pixelSize;
uniform vec2 resolution;

void main()
{
    // Convert normalized coordinates (0.0 -> 1.0) to actual physical pixel coordinates
    vec2 pixelCoord = gl_TexCoord[0].xy * resolution;
    
    // Group physical pixels into mosaic blocks based on pixelSize
    vec2 pixelatedCoord = floor(pixelCoord / pixelSize) * pixelSize;
    
    // Convert back to normalized coordinates and sample the original texture
    gl_FragColor = texture2D(texture, pixelatedCoord / resolution);
}
