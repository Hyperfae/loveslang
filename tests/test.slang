import love;

[Shader("vertex")]
void vertexMain() {
    love::vertex::lambda((float4x4 transformProjection, float4 localPos) => {
        return mul(transformProjection, localPos);
    });
}

uniform highp float time;

Sampler2D tex2;
[Shader("pixel")]
void pixelMain() {
    using namespace love::pixel;
    lambda((float4 vtxcolor, Sampler2D tex, float2 uv, float2 fragCoord) => {
        float2 iuv = uv;
        uv.x += (sin((uv.y + time) * float.getPi() * 4)) / 4;
        uv.y += (sin(uv.x * float.getPi() * 4)) / 4;
        uv.y += (sin(iuv.x * 16)) / 8;
        // uv = lerp(iuv, uv, 0.5);
        return vtxcolor * tex2.Sample(uv) * float4(uv.x, (( uv.y) + uv.x) / 2, uv.y, 1);
    });
}

AppendStructuredBuffer<float> intbuffer;
[Shader("compute")]
[NumThreads(1,1,1)]
void computeMain(uint3 threadID : SV_DispatchThreadID) {
    intbuffer.Append(1);
}
