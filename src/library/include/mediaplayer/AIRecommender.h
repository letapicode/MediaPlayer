#ifndef MEDIAPLAYER_AIRECOMMENDER_H
#define MEDIAPLAYER_AIRECOMMENDER_H

#include <vector>

namespace mediaplayer {

struct MediaMetadata;
class LibraryDB;

class AIRecommender {
public:
  virtual ~AIRecommender() = default;
  virtual std::vector<MediaMetadata> recommend(const LibraryDB &db) = 0;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_AIRECOMMENDER_H
