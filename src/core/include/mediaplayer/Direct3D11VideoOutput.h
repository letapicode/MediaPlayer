#ifndef MEDIAPLAYER_DIRECT3D11VIDEOOUTPUT_H
#define MEDIAPLAYER_DIRECT3D11VIDEOOUTPUT_H

#include "ComPtr.h"
#include "VideoFrame.h"
#include "VideoOutput.h"
#ifdef _WIN32
#include <d3d11.h>
#include <dxgi1_2.h>
#include <windows.h>
#endif

namespace mediaplayer {

#ifdef _WIN32
class Direct3D11VideoOutput : public VideoOutput {
public:
  Direct3D11VideoOutput();
  ~Direct3D11VideoOutput() override;

  bool init(int width, int height) override;
  void shutdown() override;
  void displayFrame(const VideoFrame &frame);

private:
  bool createDeviceAndSwapChain(int width, int height);
  bool createResources(int width, int height);

  HWND m_hwnd{nullptr};
  ComPtr<ID3D11Device> m_device;
  ComPtr<ID3D11DeviceContext> m_context;
  ComPtr<IDXGISwapChain> m_swapChain;
  ComPtr<ID3D11RenderTargetView> m_rtv;
  ComPtr<ID3D11Texture2D> m_yTexture;
  ComPtr<ID3D11Texture2D> m_uTexture;
  ComPtr<ID3D11Texture2D> m_vTexture;
  ComPtr<ID3D11ShaderResourceView> m_ySrv;
  ComPtr<ID3D11ShaderResourceView> m_uSrv;
  ComPtr<ID3D11ShaderResourceView> m_vSrv;
  ComPtr<ID3D11SamplerState> m_sampler;
  ComPtr<ID3D11VertexShader> m_vs;
  ComPtr<ID3D11PixelShader> m_ps;
  ComPtr<ID3D11InputLayout> m_layout;
  ComPtr<ID3D11Buffer> m_vbo;
  int m_width{0};
  int m_height{0};
};
#endif

} // namespace mediaplayer

#endif // MEDIAPLAYER_DIRECT3D11VIDEOOUTPUT_H
