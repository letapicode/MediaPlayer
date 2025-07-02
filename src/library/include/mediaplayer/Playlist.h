#ifndef MEDIAPLAYER_PLAYLIST_H
#define MEDIAPLAYER_PLAYLIST_H

#include <string>
#include <vector>

namespace mediaplayer {

class LibraryDB;

class Playlist {
public:
  explicit Playlist(std::string name = "");

  const std::string &name() const;
  void setName(const std::string &name);

  const std::vector<std::string> &items() const;
  void addItem(const std::string &path);
  bool removeItem(const std::string &path);
  void clear();
  std::size_t size() const;

  bool save(LibraryDB &db) const;
  static Playlist load(LibraryDB &db, const std::string &name);

private:
  std::string m_name;
  std::vector<std::string> m_items;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_PLAYLIST_H
