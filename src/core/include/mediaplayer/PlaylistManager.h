#ifndef MEDIAPLAYER_PLAYLISTMANAGER_H
#define MEDIAPLAYER_PLAYLISTMANAGER_H

#include <string>
#include <vector>

namespace mediaplayer {

class PlaylistManager {
public:
  void set(const std::vector<std::string> &paths);
  void add(const std::string &path);
  void clear();
  std::string next();
  bool empty() const;
  void reset();
  void setShuffle(bool enabled);
  bool shuffle() const;

private:
  std::vector<std::string> m_items;
  std::vector<size_t> m_order;
  size_t m_index{0};
  bool m_shuffle{false};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_PLAYLISTMANAGER_H
