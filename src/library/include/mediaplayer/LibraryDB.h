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

private:
  bool insertMedia(const std::string &path, const std::string &title, const std::string &artist,
                   const std::string &album);

private:
  std::string m_path;
  sqlite3 *m_db{nullptr};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_LIBRARYDB_H
