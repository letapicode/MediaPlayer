#ifndef MEDIAPLAYER_SIMPLEFFT_H
#define MEDIAPLAYER_SIMPLEFFT_H

#include <cmath>
#include <complex>
#include <vector>

namespace mediaplayer {

inline std::vector<float> simpleFFT(const int16_t *samples, size_t count) {
  size_t n = 1;
  while (n < count)
    n <<= 1; // next power of two

  std::vector<std::complex<double>> data(n);
  for (size_t i = 0; i < count; ++i)
    data[i] = static_cast<double>(samples[i]);
  for (size_t i = count; i < n; ++i)
    data[i] = 0.0;

  const double pi = std::acos(-1.0);
  for (size_t len = 1; len < n; len <<= 1) {
    double angle = -pi / static_cast<double>(len);
    std::complex<double> wlen{std::cos(angle), std::sin(angle)};
    for (size_t i = 0; i < n; i += 2 * len) {
      std::complex<double> w{1.0, 0.0};
      for (size_t j = 0; j < len; ++j) {
        auto u = data[i + j];
        auto v = data[i + j + len] * w;
        data[i + j] = u + v;
        data[i + j + len] = u - v;
        w *= wlen;
      }
    }
  }

  std::vector<float> out(n / 2);
  for (size_t i = 0; i < out.size(); ++i)
    out[i] = static_cast<float>(std::abs(data[i]));
  return out;
}

} // namespace mediaplayer

#endif // MEDIAPLAYER_SIMPLEFFT_H
