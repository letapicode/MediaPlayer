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

  // Bit-reversal permutation
  size_t j = 0;
  for (size_t i = 1; i < n; ++i) {
    size_t bit = n >> 1;
    while (j & bit) {
      j ^= bit;
      bit >>= 1;
    }
    j ^= bit;
    if (i < j)
      std::swap(data[i], data[j]);
  }

  const double pi = std::acos(-1.0);
  for (size_t len = 2; len <= n; len <<= 1) {
    double angle = -2.0 * pi / static_cast<double>(len);
    std::complex<double> wlen{std::cos(angle), std::sin(angle)};
    for (size_t i = 0; i < n; i += len) {
      std::complex<double> w{1.0, 0.0};
      for (size_t k = 0; k < len / 2; ++k) {
        auto u = data[i + k];
        auto v = data[i + k + len / 2] * w;
        data[i + k] = u + v;
        data[i + k + len / 2] = u - v;
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
