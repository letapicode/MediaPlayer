#include "mediaplayer/RandomAIRecommender.h"

namespace mediaplayer {

RandomAIRecommender::RandomAIRecommender(std::size_t count) : m_count(count) {}

std::vector<MediaMetadata> RandomAIRecommender::recommend(const LibraryDB &db) {
  auto all = db.allMedia();
  if (all.empty() || m_count == 0)
    return {};
  if (all.size() <= m_count)
    return all;
  std::shuffle(all.begin(), all.end(), m_rng);
  all.resize(m_count);
  return all;
}

} // namespace mediaplayer
