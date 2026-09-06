import love;

[Shader("vertex")]
void vertexMain() {
    love::vertex::lambda((float4x4 transformProjection, float4 localPos) => {
        return mul(transformProjection, localPos);
    });
}

[Shader("pixel")]
void pixelMain() {
    love::pixel::lambda((float4 vtxcolor, Sampler2D tex, float2 uv, float2 fragCoord) => {
        return vtxcolor * tex.Sample(uv) * float4(uv, 1, 1);
    });
}
