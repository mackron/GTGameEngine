

<!-- Default material shaders. These shaders will be used when the applicable shader is omitted in a material definition. -->
<shader id="Material_DefaultDiffuse">
    float4 Diffuse()
    {
        return float4(1.0, 1.0, 1.0);
    }
</shader>

<shader id="Material_DefaultEmissive">
    float3 Emissive()
    {
        return float4(0.0, 0.0, 0.0);
    }
</shader>

<shader id="Material_DefaultShininess">
    float Shininess()
    {
        return 0.5;
    }
</shader>


<shader id="Engine_NoDiffuse_FS">
    float3 Diffuse()
    {
        return float3(0.0f, 0.0f, 0.0f);
    }
</shader>

<shader id="Engine_NoEmissive_FS">
    float3 Emissive()
    {
        return float3(0.0f, 0.0f, 0.0f);
    }
</shader>

<shader id="Engine_SimpleDiffuse2_FS">
    uniform float3 DiffuseColour;
    
    float3 Diffuse()
    {
        return DiffuseColour;
    }
</shader>

<shader id="Engine_SimpleEmissive2_FS">
    uniform float3 EmissiveColour;
    
    float3 Emissive()
    {
        return EmissiveColour;
    }
</shader>

<shader id="Engine_TexturedDiffuse_FS">
    uniform sampler2D DiffuseTexture;
    
    float3 Diffuse()
    {
        return tex2D(DiffuseTexture, IN.TexCoord).rgb;
    }
</shader>
