

<!-- Default material shaders. These shaders will be used when the applicable shader is omitted in a material definition. -->
<shader id="Material_DefaultDiffuse">
    float3 Diffuse()
    {
        return float4(1.0, 1.0, 1.0);
    }
</shader>

<shader id="Material_DefaultEmissive">
    float3 Emissive()
    {
        return float3(0.0, 0.0, 0.0);
    }
</shader>

<shader id="Material_DefaultShininess">
    float Shininess()
    {
        return 0.5;
    }
</shader>

<shader id="Material_DefaultNormal">
    float3 Normal()
    {
        return float3(0.0f, 0.0f, 1.0f);
    }
</shader>


<shader id="Material_NoDiffuse">
    float3 Diffuse()
    {
        return float3(0.0f, 0.0f, 0.0f);
    }
</shader>

<shader id="Material_NoEmissive">
    float3 Emissive()
    {
        return float3(0.0f, 0.0f, 0.0f);
    }
</shader>

<shader id="Material_NoShininess">
    float Shininess()
    {
        return 0.0;
    }
</shader>


<shader id="Material_SimpleDiffuse">
    uniform float3 DiffuseColour;
    
    float3 Diffuse()
    {
        return DiffuseColour;
    }
</shader>

<shader id="Material_SimpleEmissive">
    uniform float3 EmissiveColour;
    
    float3 Emissive()
    {
        return EmissiveColour;
    }
</shader>

<shader id="Material_SimpleShininess">
    uniform float ShininessFactor;
    
    float Shininess()
    {
        return ShininessFactor;
    }
</shader>


<shader id="Material_TexturedDiffuse">
    uniform sampler2D DiffuseTexture;
    
    float3 Diffuse()
    {
        return tex2D(DiffuseTexture, IN.TexCoord).rgb;
    }
</shader>

<shader id="Material_TexturedNormal">   <!-- Use this one for standard normal mapping using a texture. -->
    uniform sampler2D NormalMap;
    
    float3 Normal()
    {
        return normalize(tex2D(NormalMap, IN.TexCoord).rgb * 2.0 - 1.0);
    }
</shader>

<shader id="Material_TexturedEmissive">   <!-- Use this one for standard normal mapping using a texture. -->
    uniform sampler2D EmissiveMap;
    
    float3 Emissive()
    {
        return tex2D(EmissiveMap, IN.TexCoord).rgb;
    }
</shader>
