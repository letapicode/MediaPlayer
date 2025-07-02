#include "mediaplayer/RandomRecommender.h"
#include "mediaplayer/LibraryDB.h"
#include <algorithm>

namespace mediaplayer {

RandomRecommender::RandomRecommender(size_t count) : m_count(count) {}

std::vector<MediaMetadata> RandomRecommender::recommend(const LibraryDB &db) {
  auto all = db.allMedia();
  if (all.size() <= m_count)
    return all;
  std::shuffle(all.begin(), all.end(), m_rng);
  all.resize(m_count);
  return all;
}

} // namespace mediaplayer
