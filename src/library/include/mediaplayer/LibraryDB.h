#ifndef MEDIAPLAYER_LIBRARYDB_H
#define MEDIAPLAYER_LIBRARYDB_H

#include <sqlite3.h>
#include <string>

namespace mediaplayer {

class LibraryDB {
public:
  explicit LibraryDB(const std::string &path = "media_library.db");
  ~LibraryDB();

  bool open();
  void close();
  bool initSchema();
  bool scanDirectory(const std::string &directory);

  // Insert a media entry directly. Useful for tests or manual additions.
  bool addMedia(const std::string &path, const std::string &title, const std::string &artist,
                const std::string &album);

  // Update metadata for an existing media entry identified by path.
  bool updateMedia(const std::string &path, const std::string &title, const std::string &artist,
                   const std::string &album);

  // Remove a media item from the database by path.
  bool removeMedia(const std::string &path);

private:
  bool insertMedia(const std::string &path, const std::string &title, const std::string &artist,
                   const std::string &album, int duration = 0, int width = 0, int height = 0);

private:
  std::string m_path;
  sqlite3 *m_db{nullptr};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_LIBRARYDB_H
