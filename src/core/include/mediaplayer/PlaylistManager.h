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

private:
  std::vector<std::string> m_items;
  size_t m_index{0};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_PLAYLISTMANAGER_H
