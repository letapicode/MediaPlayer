#include "mediaplayer/PlaylistManager.h"

namespace mediaplayer {

void PlaylistManager::set(const std::vector<std::string> &paths) {
  m_items = paths;
  m_index = 0;
}

void PlaylistManager::add(const std::string &path) { m_items.push_back(path); }

void PlaylistManager::clear() {
  m_items.clear();
  m_index = 0;
}

std::string PlaylistManager::next() {
  if (m_index < m_items.size()) {
    return m_items[m_index++];
  }
  return {};
}

bool PlaylistManager::empty() const { return m_index >= m_items.size(); }

void PlaylistManager::reset() { m_index = 0; }

} // namespace mediaplayer
