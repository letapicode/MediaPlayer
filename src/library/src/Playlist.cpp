#include "mediaplayer/Playlist.h"
#include "mediaplayer/LibraryDB.h"
#include <algorithm>

namespace mediaplayer {

Playlist::Playlist(std::string name) : m_name(std::move(name)) {}

const std::string &Playlist::name() const { return m_name; }

void Playlist::setName(const std::string &name) { m_name = name; }

const std::vector<std::string> &Playlist::items() const { return m_items; }

void Playlist::addItem(const std::string &path) { m_items.push_back(path); }

bool Playlist::removeItem(const std::string &path) {
  auto it = std::find(m_items.begin(), m_items.end(), path);
  if (it == m_items.end())
    return false;
  m_items.erase(it);
  return true;
}

void Playlist::clear() { m_items.clear(); }

std::size_t Playlist::size() const { return m_items.size(); }

bool Playlist::save(LibraryDB &db) const { return db.savePlaylist(*this); }

Playlist Playlist::load(LibraryDB &db, const std::string &name) { return db.loadPlaylist(name); }

} // namespace mediaplayer
