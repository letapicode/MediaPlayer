#include "mediaplayer/MetalVideoOutput.h"

#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <QuartzCore/CAMetalLayer.h>

namespace mediaplayer {

MetalVideoOutput::MetalVideoOutput() = default;

MetalVideoOutput::~MetalVideoOutput() { shutdown(); }

static const char *kShaderSrc = R"(
using namespace metal;
struct VertexOut {
    float4 position [[position]];
    float2 texCoord;
};
vertex VertexOut vertexShader(uint vertexID [[vertex_id]]) {
    float2 pos[4] = { {-1.0, -1.0}, {1.0, -1.0}, {-1.0, 1.0}, {1.0, 1.0} };
    float2 tex[4] = { {0.0,1.0}, {1.0,1.0}, {0.0,0.0}, {1.0,0.0} };
    VertexOut out;
    out.position = float4(pos[vertexID], 0.0, 1.0);
    out.texCoord = tex[vertexID];
    return out;
}
fragment float4 fragmentShader(VertexOut in [[stage_in]],
                               texture2d<float> texY [[texture(0)]],
                               texture2d<float> texU [[texture(1)]],
                               texture2d<float> texV [[texture(2)]],
                               sampler s [[sampler(0)]]) {
    float y = texY.sample(s, in.texCoord).r;
    float u = texU.sample(s, in.texCoord).r - 0.5;
    float v = texV.sample(s, in.texCoord).r - 0.5;
    float3 rgb;
    rgb.r = y + 1.402 * v;
    rgb.g = y - 0.344136 * u - 0.714136 * v;
    rgb.b = y + 1.772 * u;
    return float4(rgb, 1.0);
}
)";

bool MetalVideoOutput::init(int width, int height) {
  @autoreleasepool {
    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    if (!device)
      return false;
    m_device = (__bridge_retained void *)device;

    CAMetalLayer *layer = [CAMetalLayer layer];
    layer.device = device;
    layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    layer.framebufferOnly = YES;
    m_layer = (__bridge_retained void *)layer;

    id<MTLCommandQueue> queue = [device newCommandQueue];
    if (!queue)
      return false;
    m_commandQueue = (__bridge_retained void *)queue;

    NSError *error = nil;
    id<MTLLibrary> lib = [device newLibraryWithSource:[NSString stringWithUTF8String:kShaderSrc]
                                              options:nil
                                                error:&error];
    if (!lib) {
      NSLog(@"Metal shader error: %@", error);
      return false;
    }
    id<MTLFunction> vs = [lib newFunctionWithName:@"vertexShader"];
    id<MTLFunction> fs = [lib newFunctionWithName:@"fragmentShader"];
    if (!vs || !fs)
      return false;

    MTLRenderPipelineDescriptor *desc = [[MTLRenderPipelineDescriptor alloc] init];
    desc.vertexFunction = vs;
    desc.fragmentFunction = fs;
    desc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
    id<MTLRenderPipelineState> ps = [device newRenderPipelineStateWithDescriptor:desc error:&error];
    if (!ps) {
      NSLog(@"Pipeline error: %@", error);
      return false;
    }
    m_pipelineState = (__bridge_retained void *)ps;

    MTLTextureDescriptor *yDesc =
        [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatR8Unorm
                                                           width:width
                                                          height:height
                                                       mipmapped:NO];
    m_textureY = (__bridge_retained void *)[device newTextureWithDescriptor:yDesc];

    MTLTextureDescriptor *uDesc =
        [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatR8Unorm
                                                           width:width / 2
                                                          height:height / 2
                                                       mipmapped:NO];
    m_textureU = (__bridge_retained void *)[device newTextureWithDescriptor:uDesc];
    m_textureV = (__bridge_retained void *)[device newTextureWithDescriptor:uDesc];
    return true;
  }
}

void MetalVideoOutput::shutdown() {
  @autoreleasepool {
    if (m_textureY) {
      id tmp = (__bridge_transfer id)m_textureY;
      (void)tmp;
      m_textureY = nullptr;
    }
    if (m_textureU) {
      id tmp = (__bridge_transfer id)m_textureU;
      (void)tmp;
      m_textureU = nullptr;
    }
    if (m_textureV) {
      id tmp = (__bridge_transfer id)m_textureV;
      (void)tmp;
      m_textureV = nullptr;
    }
    if (m_pipelineState) {
      id tmp = (__bridge_transfer id)m_pipelineState;
      (void)tmp;
      m_pipelineState = nullptr;
    }
    if (m_commandQueue) {
      id tmp = (__bridge_transfer id)m_commandQueue;
      (void)tmp;
      m_commandQueue = nullptr;
    }
    if (m_layer) {
      id tmp = (__bridge_transfer id)m_layer;
      (void)tmp;
      m_layer = nullptr;
    }
    if (m_device) {
      id tmp = (__bridge_transfer id)m_device;
      (void)tmp;
      m_device = nullptr;
    }
  }
}

void MetalVideoOutput::displayFrame(const uint8_t *, int) {
  // Unused; prefer YUV interface
}

void MetalVideoOutput::displayFrame(const VideoFrame &frame) {
  @autoreleasepool {
    CAMetalLayer *layer = (__bridge CAMetalLayer *)m_layer;
    id<MTLCommandQueue> queue = (__bridge id)m_commandQueue;
    id<MTLRenderPipelineState> ps = (__bridge id)m_pipelineState;
    id<MTLTexture> texY = (__bridge id)m_textureY;
    id<MTLTexture> texU = (__bridge id)m_textureU;
    id<MTLTexture> texV = (__bridge id)m_textureV;
    if (!layer)
      return;
    id<CAMetalDrawable> drawable = [layer nextDrawable];
    if (!drawable)
      return;

    MTLRegion regionY = MTLRegionMake2D(0, 0, frame.width, frame.height);
    [texY replaceRegion:regionY
            mipmapLevel:0
              withBytes:frame.data[0]
            bytesPerRow:frame.linesize[0]];
    MTLRegion regionU = MTLRegionMake2D(0, 0, frame.width / 2, frame.height / 2);
    [texU replaceRegion:regionU
            mipmapLevel:0
              withBytes:frame.data[1]
            bytesPerRow:frame.linesize[1]];
    MTLRegion regionV = MTLRegionMake2D(0, 0, frame.width / 2, frame.height / 2);
    [texV replaceRegion:regionV
            mipmapLevel:0
              withBytes:frame.data[2]
            bytesPerRow:frame.linesize[2]];

    id<MTLCommandBuffer> cmd = [queue commandBuffer];

    MTLRenderPassDescriptor *rp = [MTLRenderPassDescriptor renderPassDescriptor];
    rp.colorAttachments[0].texture = drawable.texture;
    rp.colorAttachments[0].loadAction = MTLLoadActionClear;
    rp.colorAttachments[0].storeAction = MTLStoreActionStore;
    rp.colorAttachments[0].clearColor = MTLClearColorMake(0, 0, 0, 1);

    id<MTLRenderCommandEncoder> enc = [cmd renderCommandEncoderWithDescriptor:rp];
    [enc setRenderPipelineState:ps];
    [enc setFragmentTexture:texY atIndex:0];
    [enc setFragmentTexture:texU atIndex:1];
    [enc setFragmentTexture:texV atIndex:2];
    [enc drawPrimitives:MTLPrimitiveTypeTriangleStrip vertexStart:0 vertexCount:4];
    [enc endEncoding];
    [cmd presentDrawable:drawable];
    [cmd commit];
  }
}

} // namespace mediaplayer
