
<shader id="Engine_SimpleLine_VS">
    attribute vec3 VertexInput_Position;
   
    uniform mat4 MVPMatrix;
    
    void main()
    {
        gl_Position = MVPMatrix * vec4(VertexInput_Position, 1.0);
    }
</shader>
    
<shader id="Engine_SimpleLine_FS">
    uniform vec3 Color;
    
    void main()
    {
        gl_FragData[0] = vec4(Color, 1.0);
    }
</shader>