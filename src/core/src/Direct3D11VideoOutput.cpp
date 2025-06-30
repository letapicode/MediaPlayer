#ifdef _WIN32
#include "mediaplayer/Direct3D11VideoOutput.h"
#include <d3dcompiler.h>
#include <iostream>

namespace mediaplayer {

namespace {

const char *kVertexShader = R"(
struct VSIn {
  float2 pos : POSITION;
  float2 uv : TEXCOORD0;
};
struct VSOut {
  float4 pos : SV_POSITION;
  float2 uv : TEXCOORD0;
};
VSOut main(VSIn input) {
  VSOut o;
  o.pos = float4(input.pos, 0.0, 1.0);
  o.uv = input.uv;
  return o;
})";

const char *kPixelShader = R"(
Texture2D texY : register(t0);
Texture2D texU : register(t1);
Texture2D texV : register(t2);
SamplerState samp : register(s0);
float4 main(float4 pos : SV_POSITION, float2 uv : TEXCOORD0) : SV_Target {
  float y = texY.Sample(samp, uv).r;
  float u = texU.Sample(samp, uv).r - 0.5;
  float v = texV.Sample(samp, uv).r - 0.5;
  float r = y + 1.402 * v;
  float g = y - 0.344136 * u - 0.714136 * v;
  float b = y + 1.772 * u;
  return float4(r, g, b, 1.0);
})";

struct Vertex {
  float pos[2];
  float uv[2];
};

} // namespace

Direct3D11VideoOutput::Direct3D11VideoOutput() = default;

Direct3D11VideoOutput::~Direct3D11VideoOutput() { shutdown(); }

bool Direct3D11VideoOutput::createDeviceAndSwapChain(int width, int height) {
  WNDCLASSW wc = {};
  wc.lpfnWndProc = DefWindowProcW;
  wc.hInstance = GetModuleHandleW(nullptr);
  wc.lpszClassName = L"MediaPlayerWindow";
  RegisterClassW(&wc);
  m_hwnd = CreateWindowW(wc.lpszClassName, L"MediaPlayer", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                         CW_USEDEFAULT, width, height, nullptr, nullptr, wc.hInstance, nullptr);
  if (!m_hwnd)
    return false;
  ShowWindow(m_hwnd, SW_SHOW);

  DXGI_SWAP_CHAIN_DESC sd = {};
  sd.BufferCount = 2;
  sd.BufferDesc.Width = width;
  sd.BufferDesc.Height = height;
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = m_hwnd;
  sd.SampleDesc.Count = 1;
  sd.Windowed = TRUE;

  UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
  D3D_FEATURE_LEVEL level;
  const D3D_FEATURE_LEVEL levels[] = {D3D_FEATURE_LEVEL_11_0};
  if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
                                           levels, 1, D3D11_SDK_VERSION, &sd, &m_swapChain,
                                           &m_device, &level, &m_context))) {
    std::cerr << "Failed to create D3D11 device\n";
    return false;
  }
  ComPtr<ID3D11Texture2D> backBuf;
  if (FAILED(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuf))))
    return false;
  if (FAILED(m_device->CreateRenderTargetView(backBuf.get(), nullptr, &m_rtv)))
    return false;
  return true;
}

bool Direct3D11VideoOutput::createResources(int width, int height) {
  D3D11_TEXTURE2D_DESC desc = {};
  desc.Width = width;
  desc.Height = height;
  desc.MipLevels = 1;
  desc.ArraySize = 1;
  desc.Format = DXGI_FORMAT_R8_UNORM;
  desc.SampleDesc.Count = 1;
  desc.Usage = D3D11_USAGE_DYNAMIC;
  desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

  if (FAILED(m_device->CreateTexture2D(&desc, nullptr, &m_yTexture)))
    return false;
  if (FAILED(m_device->CreateTexture2D(&desc, nullptr, &m_uTexture)))
    return false;
  if (FAILED(m_device->CreateTexture2D(&desc, nullptr, &m_vTexture)))
    return false;

  D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
  srvDesc.Format = desc.Format;
  srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  srvDesc.Texture2D.MipLevels = 1;

  if (FAILED(m_device->CreateShaderResourceView(m_yTexture.get(), &srvDesc, &m_ySrv)))
    return false;
  if (FAILED(m_device->CreateShaderResourceView(m_uTexture.get(), &srvDesc, &m_uSrv)))
    return false;
  if (FAILED(m_device->CreateShaderResourceView(m_vTexture.get(), &srvDesc, &m_vSrv)))
    return false;

  D3D11_SAMPLER_DESC sampDesc = {};
  sampDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
  sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
  if (FAILED(m_device->CreateSamplerState(&sampDesc, &m_sampler)))
    return false;

  ComPtr<ID3DBlob> vsBlob;
  ComPtr<ID3DBlob> psBlob;
  ComPtr<ID3DBlob> errBlob;
  if (FAILED(D3DCompile(kVertexShader, strlen(kVertexShader), nullptr, nullptr, nullptr, "main",
                        "vs_5_0", 0, 0, &vsBlob, &errBlob))) {
    if (errBlob)
      std::cerr << (char *)errBlob->GetBufferPointer() << std::endl;
    return false;
  }
  if (FAILED(D3DCompile(kPixelShader, strlen(kPixelShader), nullptr, nullptr, nullptr, "main",
                        "ps_5_0", 0, 0, &psBlob, &errBlob))) {
    if (errBlob)
      std::cerr << (char *)errBlob->GetBufferPointer() << std::endl;
    return false;
  }
  if (FAILED(m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
                                          nullptr, &m_vs)))
    return false;
  if (FAILED(m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(),
                                         nullptr, &m_ps)))
    return false;
  D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
      {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
      {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0},
  };
  if (FAILED(m_device->CreateInputLayout(layoutDesc, 2, vsBlob->GetBufferPointer(),
                                         vsBlob->GetBufferSize(), &m_layout)))
    return false;

  Vertex verts[] = {{{-1.f, -1.f}, {0.f, 1.f}},
                    {{1.f, -1.f}, {1.f, 1.f}},
                    {{-1.f, 1.f}, {0.f, 0.f}},
                    {{1.f, 1.f}, {1.f, 0.f}}};
  D3D11_BUFFER_DESC vbDesc = {};
  vbDesc.ByteWidth = sizeof(verts);
  vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
  vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  D3D11_SUBRESOURCE_DATA vbData = {};
  vbData.pSysMem = verts;
  if (FAILED(m_device->CreateBuffer(&vbDesc, &vbData, &m_vbo)))
    return false;

  return true;
}

bool Direct3D11VideoOutput::init(int width, int height) {
  if (!createDeviceAndSwapChain(width, height))
    return false;
  if (!createResources(width, height))
    return false;
  m_width = width;
  m_height = height;
  return true;
}

void Direct3D11VideoOutput::shutdown() {
  m_vbo.reset();
  m_layout.reset();
  m_vs.reset();
  m_ps.reset();
  m_sampler.reset();
  m_ySrv.reset();
  m_uSrv.reset();
  m_vSrv.reset();
  m_yTexture.reset();
  m_uTexture.reset();
  m_vTexture.reset();
  m_rtv.reset();
  m_swapChain.reset();
  m_context.reset();
  m_device.reset();
  if (m_hwnd) {
    DestroyWindow(m_hwnd);
    m_hwnd = nullptr;
  }
}

void Direct3D11VideoOutput::displayFrame(const VideoFrame &frame) {
  if (!m_context || !m_swapChain)
    return;

  D3D11_MAPPED_SUBRESOURCE mapped{};
  if (SUCCEEDED(m_context->Map(m_yTexture.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
    for (int y = 0; y < frame.height; ++y) {
      memcpy(static_cast<uint8_t *>(mapped.pData) + mapped.RowPitch * y,
             frame.y + frame.linesizeY * y, frame.width);
    }
    m_context->Unmap(m_yTexture.get(), 0);
  }
  int chromaH = frame.height / 2;
  int chromaW = frame.width / 2;
  if (SUCCEEDED(m_context->Map(m_uTexture.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
    for (int y = 0; y < chromaH; ++y) {
      memcpy(static_cast<uint8_t *>(mapped.pData) + mapped.RowPitch * y,
             frame.u + frame.linesizeU * y, chromaW);
    }
    m_context->Unmap(m_uTexture.get(), 0);
  }
  if (SUCCEEDED(m_context->Map(m_vTexture.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
    for (int y = 0; y < chromaH; ++y) {
      memcpy(static_cast<uint8_t *>(mapped.pData) + mapped.RowPitch * y,
             frame.v + frame.linesizeV * y, chromaW);
    }
    m_context->Unmap(m_vTexture.get(), 0);
  }

  ID3D11RenderTargetView *rt = m_rtv.get();
  m_context->OMSetRenderTargets(1, &rt, nullptr);
  D3D11_VIEWPORT vp = {0.f, 0.f, static_cast<float>(m_width), static_cast<float>(m_height),
                       0.f, 1.f};
  m_context->RSSetViewports(1, &vp);
  UINT stride = sizeof(Vertex);
  UINT offset = 0;
  ID3D11Buffer *vb = m_vbo.get();
  m_context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
  m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
  m_context->IASetInputLayout(m_layout.get());
  m_context->VSSetShader(m_vs.get(), nullptr, 0);
  ID3D11ShaderResourceView *srvs[] = {m_ySrv.get(), m_uSrv.get(), m_vSrv.get()};
  m_context->PSSetShaderResources(0, 3, srvs);
  ID3D11SamplerState *sam = m_sampler.get();
  m_context->PSSetSamplers(0, 1, &sam);
  m_context->PSSetShader(m_ps.get(), nullptr, 0);
  m_context->Draw(4, 0);
  m_swapChain->Present(1, 0);
}

} // namespace mediaplayer
#endif
