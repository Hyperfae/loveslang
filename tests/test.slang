import love;
import testlib;

//[love_AttributeName("MyVarying")]
//out float4 VaryingOut_MyVarying;
[Shader("vertex")]
void vertexMain() {
    love::vertex::lambda((float4x4 transformProjection, float4 localPos) => {
        //VaryingOut_MyVarying = float4(localPos.xy, 0, 1);
        return mul(transformProjection, localPos);
    });
}
uniform highp float time;
uniform highp float ampl;

//[love_AttributeName("MyVarying")]
//in float4 VaryingIn_MyVarying;

Sampler2D tex2;
[Shader("pixel")]
void pixelMain() {
    using namespace love::pixel;
    lambda((float4 vtxcolor, Sampler2D tex, float2 uv, float2 fragCoord) => {
        float2 iuv = uv;
        uv.x += (sin((uv.y + time) * float.getPi() * 4)) / testlib::getFour();
        uv.y += (sin(uv.x * float.getPi() * 4)) / 4;
        uv.y += (sin(iuv.x * 16)) / 8;
        uv = lerp(iuv, uv, ampl);
        return vtxcolor * tex2.Sample(uv) * float4(uv.x, (( uv.y) + uv.x) / 2, uv.y, 1);// + VaryingIn_MyVarying;
    });
}

AppendStructuredBuffer<float> intbuffer;
[Shader("compute")]
[NumThreads(1,1,1)]
void computeMain(uint3 threadID : SV_DispatchThreadID) {
    intbuffer.Append(1);
}
