

<!-- Default material shaders. These shaders will be used when the applicable shader is omitted in a material definition. -->
<shader id="Material_DefaultDiffuse">
    vec3 Diffuse()
    {
        return vec3(1.0, 1.0, 1.0);
    }
</shader>

<shader id="Material_DefaultEmissive">
    vec3 Emissive()
    {
        return vec3(0.0, 0.0, 0.0);
    }
</shader>

<shader id="Material_DefaultShininess">
    float Shininess()
    {
        return 0.5;
    }
</shader>

<shader id="Material_DefaultNormal">
    vec3 Normal()
    {
        return vec3(0.0f, 0.0f, 1.0f);
    }
</shader>


<shader id="Material_NoDiffuse">
    vec3 Diffuse()
    {
        return vec3(0.0f, 0.0f, 0.0f);
    }
</shader>

<shader id="Material_NoEmissive">
    vec3 Emissive()
    {
        return vec3(0.0f, 0.0f, 0.0f);
    }
</shader>

<shader id="Material_NoShininess">
    float Shininess()
    {
        return 0.0;
    }
</shader>


<shader id="Material_SimpleDiffuse">
    uniform vec3 DiffuseColour;
    
    vec3 Diffuse()
    {
        return DiffuseColour;
    }
</shader>

<shader id="Material_SimpleEmissive">
    uniform vec3 EmissiveColour;
    
    vec3 Emissive()
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
    
    vec3 Diffuse()
    {
        return texture2D(DiffuseTexture, VertexOutput_TexCoord).rgb;
    }
</shader>

<shader id="Material_TexturedNormal">   <!-- Use this one for standard normal mapping using a texture. -->
    uniform sampler2D NormalMap;
    
    vec3 Normal()
    {
        return normalize(texture2D(NormalMap, VertexOutput_TexCoord).rgb * 2.0 - 1.0);
    }
</shader>

<shader id="Material_TexturedEmissive">   <!-- Use this one for standard normal mapping using a texture. -->
    uniform sampler2D EmissiveMap;
    
    vec3 Emissive()
    {
        return texture2D(EmissiveMap, VertexOutput_TexCoord).rgb;
    }
</shader>
