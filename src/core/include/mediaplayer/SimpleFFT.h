#ifndef MEDIAPLAYER_SIMPLEFFT_H
#define MEDIAPLAYER_SIMPLEFFT_H

#include <complex>
#include <vector>

namespace mediaplayer {

inline std::vector<float> simpleFFT(const int16_t *samples, size_t count) {
  size_t n = count;
  std::vector<float> out(n / 2);
  const double twoPi = 6.283185307179586476925286766559;
  for (size_t k = 0; k < out.size(); ++k) {
    std::complex<double> sum{0.0, 0.0};
    for (size_t t = 0; t < n; ++t) {
      double angle = twoPi * k * t / n;
      std::complex<double> w{std::cos(angle), -std::sin(angle)};
      sum += static_cast<double>(samples[t]) * w;
    }
    out[k] = static_cast<float>(std::abs(sum));
  }
  return out;
}

} // namespace mediaplayer

#endif // MEDIAPLAYER_SIMPLEFFT_H
