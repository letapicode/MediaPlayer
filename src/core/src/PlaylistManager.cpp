#include "mediaplayer/PlaylistManager.h"

namespace mediaplayer {

void PlaylistManager::set(const std::vector<std::string> &paths) {
  m_items = paths;
  m_index = 0;
  resetShuffle();
}

void PlaylistManager::add(const std::string &path) {
  m_items.push_back(path);
  if (m_shuffle)
    m_unusedIndices.push_back(m_items.size() - 1);
}

void PlaylistManager::clear() {
  m_items.clear();
  m_index = 0;
  m_unusedIndices.clear();
}

std::string PlaylistManager::next() {
  if (m_shuffle) {
    if (m_unusedIndices.empty())
      return {};
    std::uniform_int_distribution<size_t> dist(0, m_unusedIndices.size() - 1);
    size_t pos = dist(m_rng);
    size_t idx = m_unusedIndices[pos];
    m_unusedIndices.erase(m_unusedIndices.begin() + pos);
    return m_items[idx];
  }
  if (m_index < m_items.size())
    return m_items[m_index++];
  return {};
}

bool PlaylistManager::empty() const {
  if (m_shuffle)
    return m_unusedIndices.empty();
  return m_index >= m_items.size();
}

void PlaylistManager::reset() {
  m_index = 0;
  resetShuffle();
}

void PlaylistManager::enableShuffle(bool enabled) {
  m_shuffle = enabled;
  reset();
}

bool PlaylistManager::shuffleEnabled() const { return m_shuffle; }

void PlaylistManager::resetShuffle() {
  m_unusedIndices.clear();
  if (m_shuffle)
    for (size_t i = 0; i < m_items.size(); ++i)
      m_unusedIndices.push_back(i);
}

} // namespace mediaplayer
