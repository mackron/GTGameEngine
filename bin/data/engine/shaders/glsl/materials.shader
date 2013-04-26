

<!-- Default material shaders. These shaders will be used when the applicable shader is omitted in a material definition. -->
<shader id="Material_DefaultDiffuse">
    vec4 Diffuse()
    {
        return vec4(1.0, 1.0, 1.0, 1.0);
    }
</shader>

<shader id="Material_DefaultEmissive">
    vec3 Emissive()
    {
        return vec3(0.0, 0.0, 0.0);
    }
</shader>

<shader id="Material_DefaultNormal">
    vec3 Normal()
    {
        return vec3(0.0, 0.0, 1.0);
    }
</shader>

<shader id="Material_DefaultShininess">
    float Shininess()
    {
        return 0.5;
    }
</shader>

<shader id="Material_DefaultSpecular">
    float Specular()
    {
        return 64.0;
    }
</shader>




<shader id="Material_NoDiffuse">
    vec4 Diffuse()
    {
        return vec4(0.0, 0.0, 0.0, 1.0);
    }
</shader>

<shader id="Material_NoEmissive">
    vec3 Emissive()
    {
        return vec3(0.0, 0.0, 0.0);
    }
</shader>

<shader id="Material_NoShininess">
    float Shininess()
    {
        return 0.0;
    }
</shader>

<shader id="Material_NoRefraction">
    vec3 Refraction()
    {
        return vec3(0.0, 0.0, 1.0);
    }
</shader>





<shader id="Material_SimpleDiffuse">
    uniform vec3 DiffuseColour;
    
    vec4 Diffuse()
    {
        return vec4(DiffuseColour, 1.0f);
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

<shader id="Material_SimpleSpecularPower">
    uniform float SpecularPower;
    
    float Specular()
    {
        return ShininessFactor;
    }
</shader>






<shader id="Material_TexturedDiffuse">
    uniform sampler2D DiffuseTexture;
    
    vec4 Diffuse()
    {
        return texture2D(DiffuseTexture, VertexOutput_TexCoord.xy);
    }
</shader>

<shader id="Material_TexturedNormal">   <!-- Use this one for standard normal mapping using a texture. -->
    uniform sampler2D NormalMap;
    
    vec3 Normal()
    {
        return normalize(texture2D(NormalMap, VertexOutput_TexCoord.xy).rgb * 2.0 - 1.0);
    }
</shader>

<shader id="Material_TexturedEmissive">
    uniform sampler2D EmissiveTexture;
    
    vec3 Emissive()
    {
        return texture2D(EmissiveTexture, VertexOutput_TexCoord.xy).rgb;
    }
</shader>

<shader id="Material_TexturedRefraction">
    uniform sampler2D RefractionMap;
    
    vec3 Refraction()
    {
        return normalize(texture2D(RefractionMap, VertexOutput_TexCoord.xy).rgb * 2.0 - 1.0);
    }
</shader>

