[__AttributeUsage(_AttributeTargets.Var)]
internal struct love_NoExportAttribute {}
namespace love {
    public float4 getConstantColor() {
        return getUniformPerDraw(10);
    }

    [love::NoExport]
    internal uniform float4[12] love_UniformsPerDraw;
    [ForceInline]
    internal float4 getUniformPerDraw(int index) {
        __target_switch
        {
            case glsl:
                __intrinsic_asm "love_UniformsPerDraw[$0]";
            default:
                return love_UniformsPerDraw[index];
        }
    }

    public float4x4 getTransformMatrix() {
        return transpose(float4x4(getUniformPerDraw(0), getUniformPerDraw(1), getUniformPerDraw(2), getUniformPerDraw(3)));
    }

    public float4x4 getProjectionMatrix() {
        return transpose(float4x4(getUniformPerDraw(4), getUniformPerDraw(5), getUniformPerDraw(6), getUniformPerDraw(7)));
    }

    [ForceInline]
    public float4x4 getTransformProjectionMatrix() {
        return mul(getProjectionMatrix(), getTransformMatrix());
    }
}

namespace love::vertex {

    [love::NoExport]
    layout (location = 0) in float4 VertexPosition;
    [love::NoExport]
    layout (location = 1) in float4 VertexTexCoord;
    [love::NoExport]
    layout (location = 2) in float4 VertexColor;

    [love::NoExport]
    out float4 VaryingTexCoord;
    [love::NoExport]
    out float4 VaryingColor;
    [love::NoExport]
    out float4 love_Position : SV_Position;
    [ForceInline]
    public float4 readInputVertexPosition() {
        __target_switch
        {
            #ifdef LOVESLANG
            case glsl:
                __requirePrelude("layout (location = 0) in vec4 VertexPosition;");
                __intrinsic_asm R"CODESTRING( VertexPosition )CODESTRING";
            #endif
            default:
                return VertexPosition;
        }
    }
    [ForceInline]
    public float4 readInputVertexTexCoord() {
        __target_switch
        {
            #ifdef LOVESLANG
            case glsl:
                __requirePrelude("layout (location = 1) in vec4 VertexTexCoord;");
                __intrinsic_asm R"CODESTRING( VertexTexCoord )CODESTRING";
            #endif
            default:
                return VertexTexCoord;
        }
    }
    [ForceInline]
    public float4 readInputVertexColor() {
        __target_switch
        {
            #ifdef LOVESLANG
            case glsl:
                __requirePrelude("layout (location = 2) in vec4 VertexColor;");
                __intrinsic_asm R"CODESTRING( VertexColor )CODESTRING";
            #endif
            default:
                return VertexColor;
        }
    }
    [ForceInline]
    public void writeVertexPosition(float4 position) {
        __target_switch
        {
            #ifdef LOVESLANG
            case glsl:
                __intrinsic_asm R"CODESTRING(love_Position = $0)CODESTRING";
            #endif
            default:
                love_Position = position;
        }
    }
    [ForceInline]
    public void writeVertexTexCoord(float4 color) {
        __target_switch
        {
            case glsl:
            #ifdef LOVESLANG
                __requirePrelude("out vec4 VaryingTexCoord;");
                __intrinsic_asm R"CODESTRING(VaryingTexCoord = $0)CODESTRING";
            #endif
            default:
                VaryingTexCoord = color;
        }
    }
    [ForceInline]
    public void writeVertexColor(float4 color) {
        __target_switch
        {
            case glsl:
            #ifdef LOVESLANG
                __requirePrelude("out vec4 VaryingColor;");
                __intrinsic_asm R"CODESTRING(VaryingColor = $0)CODESTRING";
            #endif
            default:
                VaryingColor = color;
        }
    }

    [love::NoExport]
    internal in int love_InstanceID : SV_InstanceID;
    [ForceInline]
    public int getInstanceID() {
        __target_switch
        {
            #ifdef LOVESLANG
            case glsl:
                __intrinsic_asm "(love_InstanceID)";
            #endif
            default:
                return love_InstanceID;
        }
    }

    public void lambda(IFunc<float4, float4x4, float4> func) {
        writeVertexColor(readInputVertexColor());
        writeVertexTexCoord(readInputVertexTexCoord());
        writeVertexPosition(func(getTransformProjectionMatrix(), readInputVertexPosition()));
    }
}

namespace love::pixel {

    [love::NoExport]
    internal out float4 love_PixelColor: SV_Target;
    [love::NoExport]
    internal uniform Sampler2D MainTex;

    [ForceInline]
    public void writePixelColor(float4 color) {
        __target_switch
        {
            #ifdef LOVESLANG
            case glsl:
                __intrinsic_asm R"CODESTRING(love_PixelColor = $0)CODESTRING";
            #endif
            default:
                love_PixelColor = color;
        }
    }

    [ForceInline]
    public void writePixelColor(float4 color, uint canvas_index) {
        __target_switch
        {
            #ifdef LOVESLANG
            case glsl:
                __intrinsic_asm R"CODESTRING(love_RenderTargets[$1] = $0)CODESTRING";
            #endif
            default:
                love_PixelColor = color;
        }
    }
    [ForceInline]
    public float4 readPixelColor() {
        __target_switch
        {
            #ifdef LOVESLANG
            case glsl:
                __intrinsic_asm R"CODESTRING( love_PixelColor )CODESTRING";
            #endif
            default:
                return love_PixelColor;
        }
    }

    [love::NoExport]
    internal in float4 VaryingColor;
    [ForceInline]
    public float4 readVertexColor() {
        __target_switch
        {
            #ifdef LOVESLANG
            case glsl:
                // __requirePrelude(R"CODESTRING(in vec4 VaryingColor;)CODESTRING");
                __intrinsic_asm R"CODESTRING( VaryingColor )CODESTRING";
            #endif
            default:
                return VaryingColor;
        }
    }



    [love::NoExport]
    internal in float4 VaryingTexCoord;
    [ForceInline]
    public float4 readVertexTexCoord() {
        __target_switch
        {
            case glsl:
            #ifdef LOVESLANG
                // __requirePrelude(R"CODESTRING(in vec4 VaryingTexCoord;)CODESTRING");
                __intrinsic_asm R"CODESTRING( VaryingTexCoord )CODESTRING";
            #endif
            default:
                return VaryingTexCoord;
        }
    }

    [ForceInline]
    public Sampler2D getMainTexture2D() {
        __target_switch
        {
            case glsl:
            #ifdef LOVESLANG
                __requirePrelude(R"CODESTRING(uniform sampler2D MainTex;)CODESTRING");
                __intrinsic_asm R"CODESTRING( MainTex )CODESTRING";
            #endif
            default:
                return MainTex;
        }
    }

    [ForceInline]
    public float4 VideoTexel(float2 coords) {
        __target_switch
        {
            #ifdef LOVESLANG
            case glsl:
                __intrinsic_asm R"CODESTRING( VideoTexel($0) )CODESTRING";
            #endif
            default:
                // TODO: Implement VideoTexel in Slang
                return float4(0, 0, 0, 1);
        }
    }

    public float2 getPixelCoord() {
        __target_switch
        {
            #ifdef LOVESLANG
            case glsl:
                __intrinsic_asm R"CODESTRING( love_PixelCoord )CODESTRING";
            #endif
            default:
                // TODO: Implement getPixelCoord in Slang
                return float2(0, 0);
        }
    }
    public void lambda(IFunc<float4, float4, Sampler2D, float2, float2> func) {
        writePixelColor(func(readVertexColor(), getMainTexture2D(), readVertexTexCoord().xy, getPixelCoord()));
    }
}


