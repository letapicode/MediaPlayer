#ifndef MEDIAPLAYER_RANDOMRECOMMENDER_H
#define MEDIAPLAYER_RANDOMRECOMMENDER_H

#include "mediaplayer/AIRecommender.h"
#include <random>

namespace mediaplayer {

class RandomRecommender : public AIRecommender {
public:
  explicit RandomRecommender(size_t count = 10);
  std::vector<MediaMetadata> recommend(const LibraryDB &db) override;

private:
  size_t m_count;
  std::mt19937 m_rng{std::random_device{}()};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_RANDOMRECOMMENDER_H
