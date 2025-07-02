#ifndef MEDIAPLAYER_RANDOMAIRECOMMENDER_H
#define MEDIAPLAYER_RANDOMAIRECOMMENDER_H

#include "mediaplayer/AIRecommender.h"
#include "mediaplayer/LibraryDB.h"
#include <cstddef>
#include <random>

namespace mediaplayer {

// Simple stub recommender that returns a random subset of media items.
class RandomAIRecommender : public AIRecommender {
public:
  explicit RandomAIRecommender(std::size_t count = 10);
  std::vector<MediaMetadata> recommend(const LibraryDB &db) override;

private:
  std::size_t m_count;
  std::mt19937 m_rng{std::random_device{}()};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_RANDOMAIRECOMMENDER_H
