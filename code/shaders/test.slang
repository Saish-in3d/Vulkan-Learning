struct VertexInput {
    [[vk::location(0)]] float2 pos;
    [[vk::location(1)]] float3 color;
};

struct VertexOutput {
    float4 position : SV_Position;
    [[vk::location(0)]] float3 color;
};

struct CameraUBO {
    float4x4 view;
    float4x4 proj;
};

struct ObjectUBO {
    float4x4 model;
};

[[vk::binding(0, 0)]] ConstantBuffer<CameraUBO> camera;
[[vk::binding(1, 0)]] ConstantBuffer<ObjectUBO> object;

[shader("vertex")]
VertexOutput vertMain(VertexInput input) {
    VertexOutput output;
    float4 worldPos = mul(object.model, float4(input.pos, 0.0, 1.0));
    float4 viewPos  = mul(camera.view, worldPos);
    output.position = mul(camera.proj, viewPos);
    output.color    = input.color;
    return output;
}

[shader("fragment")]
float4 fragMain(VertexOutput input) : SV_Target {
    return float4(input.color, 1.0);
}