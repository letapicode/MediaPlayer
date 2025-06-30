#include "mediaplayer/PlaylistManager.h"
#include <algorithm>
#include <numeric>
#include <random>

namespace mediaplayer {

void PlaylistManager::set(const std::vector<std::string> &paths) {
  m_items = paths;
  m_index = 0;
  m_order.resize(m_items.size());
  std::iota(m_order.begin(), m_order.end(), 0);
  if (m_shuffle)
    std::shuffle(m_order.begin(), m_order.end(), std::mt19937{std::random_device{}()});
}

void PlaylistManager::add(const std::string &path) {
  m_items.push_back(path);
  m_order.push_back(m_items.size() - 1);
  if (m_shuffle) {
    std::shuffle(m_order.begin(), m_order.end(), std::mt19937{std::random_device{}()});
  }
}

void PlaylistManager::clear() {
  m_items.clear();
  m_order.clear();
  m_index = 0;
}

std::string PlaylistManager::next() {
  if (m_index < m_items.size()) {
    size_t idx = m_shuffle ? m_order[m_index] : m_index;
    ++m_index;
    return m_items[idx];
  }
  return {};
}

bool PlaylistManager::empty() const { return m_index >= m_items.size(); }

void PlaylistManager::reset() {
  m_index = 0;
  if (m_shuffle && !m_order.empty()) {
    std::shuffle(m_order.begin(), m_order.end(), std::mt19937{std::random_device{}()});
  }
}

void PlaylistManager::setShuffle(bool enabled) {
  m_shuffle = enabled;
  m_order.resize(m_items.size());
  std::iota(m_order.begin(), m_order.end(), 0);
  if (m_shuffle)
    std::shuffle(m_order.begin(), m_order.end(), std::mt19937{std::random_device{}()});
}

bool PlaylistManager::shuffle() const { return m_shuffle; }

} // namespace mediaplayer
