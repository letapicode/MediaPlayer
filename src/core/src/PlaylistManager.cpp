#include "mediaplayer/PlaylistManager.h"

namespace mediaplayer {

void PlaylistManager::set(const std::vector<std::string> &paths) {
  m_items = paths;
  m_index = 0;
  resetShuffle();
  m_history.clear();
  m_historyPos = 0;
}

void PlaylistManager::add(const std::string &path) {
  m_items.push_back(path);
  if (m_shuffle)
    m_unusedIndices.push_back(m_items.size() - 1);
}

bool PlaylistManager::remove(const std::string &path) {
  bool removed = false;
  for (size_t i = 0; i < m_items.size();) {
    if (m_items[i] == path) {
      removed = true;
      m_items.erase(m_items.begin() + i);
      if (!m_shuffle && i < m_index)
        --m_index;
      for (auto it = m_unusedIndices.begin(); it != m_unusedIndices.end();) {
        if (*it == i) {
          it = m_unusedIndices.erase(it);
          continue;
        } else {
          if (*it > i)
            --(*it);
        }
        ++it;
      }
      continue; // do not increment i after erase
    }
    ++i;
  }
  return removed;
}

void PlaylistManager::clear() {
  m_items.clear();
  m_index = 0;
  m_unusedIndices.clear();
  m_history.clear();
  m_historyPos = 0;
}

std::string PlaylistManager::next() {
  size_t idx = static_cast<size_t>(-1);
  if (m_historyPos + 1 < m_history.size()) {
    ++m_historyPos;
    idx = m_history[m_historyPos];
  } else {
    if (m_shuffle) {
      if (m_unusedIndices.empty())
        return {};
      std::uniform_int_distribution<size_t> dist(0, m_unusedIndices.size() - 1);
      size_t pos = dist(m_rng);
      idx = m_unusedIndices[pos];
      m_unusedIndices.erase(m_unusedIndices.begin() + pos);
    } else {
      if (m_index >= m_items.size())
        return {};
      idx = m_index++;
    }
    m_history.push_back(idx);
    m_historyPos = m_history.size() - 1;
  }
  if (idx >= m_items.size())
    return {};
  return m_items[idx];
}

std::string PlaylistManager::previous() {
  if (m_historyPos == 0 || m_history.empty())
    return {};
  --m_historyPos;
  size_t idx = m_history[m_historyPos];
  if (idx >= m_items.size())
    return {};
  return m_items[idx];
}

bool PlaylistManager::empty() const {
  if (m_shuffle)
    return m_unusedIndices.empty();
  return m_index >= m_items.size();
}

void PlaylistManager::reset() {
  m_index = 0;
  resetShuffle();
  m_history.clear();
  m_historyPos = 0;
}

void PlaylistManager::enableShuffle(bool enabled) {
  m_shuffle = enabled;
  reset();
}

bool PlaylistManager::shuffleEnabled() const { return m_shuffle; }

bool PlaylistManager::removeAt(size_t index) {
  if (index >= m_items.size())
    return false;
  m_items.erase(m_items.begin() + index);
  if (!m_shuffle && index < m_index && m_index > 0)
    --m_index;
  for (auto it = m_history.begin(); it != m_history.end();) {
    if (*it == index) {
      size_t pos = std::distance(m_history.begin(), it);
      it = m_history.erase(it);
      if (pos <= m_historyPos && m_historyPos > 0)
        --m_historyPos;
      continue;
    } else {
      if (*it > index)
        --(*it);
    }
    ++it;
  }
  resetShuffle();
  return true;
}

bool PlaylistManager::moveItem(size_t from, size_t to) {
  if (from >= m_items.size() || to >= m_items.size() || from == to)
    return false;
  auto item = m_items[from];
  m_items.erase(m_items.begin() + from);
  m_items.insert(m_items.begin() + to, item);
  for (auto &idx : m_history) {
    if (idx == from)
      idx = to;
    else if (from < to && idx > from && idx <= to)
      --idx;
    else if (to < from && idx >= to && idx < from)
      ++idx;
  }
  resetShuffle();
  return true;
}

void PlaylistManager::resetShuffle() {
  m_unusedIndices.clear();
  if (m_shuffle)
    for (size_t i = 0; i < m_items.size(); ++i)
      m_unusedIndices.push_back(i);
}

} // namespace mediaplayer
