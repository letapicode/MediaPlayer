#ifndef MEDIAPLAYER_PLAYLISTMANAGER_H
#define MEDIAPLAYER_PLAYLISTMANAGER_H

#include <random>
#include <string>
#include <vector>

namespace mediaplayer {

class PlaylistManager {
public:
  void set(const std::vector<std::string> &paths);
  void add(const std::string &path);
  bool remove(const std::string &path);
  void clear();
  std::string next();
  bool empty() const;
  void reset();
  void enableShuffle(bool enabled);
  bool shuffleEnabled() const;
  const std::vector<std::string> &items() const { return m_items; }
  bool removeAt(size_t index);
  bool moveItem(size_t from, size_t to);

private:
  std::vector<std::string> m_items;
  size_t m_index{0};
  bool m_shuffle{false};
  std::vector<size_t> m_unusedIndices;
  std::mt19937 m_rng{std::random_device{}()};
  void resetShuffle();
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_PLAYLISTMANAGER_H
