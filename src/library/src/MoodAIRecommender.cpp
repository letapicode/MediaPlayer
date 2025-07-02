#include "mediaplayer/MoodAIRecommender.h"
#include <algorithm>

namespace mediaplayer {

MoodAIRecommender::MoodAIRecommender(std::string mood, std::size_t count)
    : m_mood(std::move(mood)), m_count(count) {}

void MoodAIRecommender::setMood(const std::string &mood) { m_mood = mood; }

std::vector<MediaMetadata> MoodAIRecommender::recommend(const LibraryDB &db) {
  std::vector<MediaMetadata> items;
  if (!m_mood.empty()) {
    items = db.search(m_mood);
  } else {
    items = db.allMedia();
  }
  if (items.empty())
    return items;
  if (items.size() > m_count) {
    std::shuffle(items.begin(), items.end(), m_rng);
    items.resize(m_count);
  }
  return items;
}

} // namespace mediaplayer
