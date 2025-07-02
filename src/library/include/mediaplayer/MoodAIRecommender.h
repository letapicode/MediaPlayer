#ifndef MEDIAPLAYER_MOODAIRECOMMENDER_H
#define MEDIAPLAYER_MOODAIRECOMMENDER_H

#include "mediaplayer/AIRecommender.h"
#include "mediaplayer/LibraryDB.h"
#include <cstddef>
#include <random>
#include <string>
#include <vector>

namespace mediaplayer {

// Stub recommender that selects tracks matching a given mood string.
// Currently it performs a simple text search over genre and shuffles
// the results.
class MoodAIRecommender : public AIRecommender {
public:
  MoodAIRecommender(std::string mood = "", std::size_t count = 10);
  void setMood(const std::string &mood);
  std::vector<MediaMetadata> recommend(const LibraryDB &db) override;

private:
  std::string m_mood;
  std::size_t m_count;
  std::mt19937 m_rng{std::random_device{}()};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_MOODAIRECOMMENDER_H
