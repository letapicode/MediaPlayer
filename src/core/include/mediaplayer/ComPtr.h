#ifndef MEDIAPLAYER_COMPTR_H
#define MEDIAPLAYER_COMPTR_H

#ifdef _WIN32
#include <utility>

namespace mediaplayer {

template <typename T> class ComPtr {
public:
  ComPtr() = default;
  ComPtr(T *p) : m_ptr(p) {}
  ComPtr(const ComPtr &) = delete;
  ComPtr &operator=(const ComPtr &) = delete;
  ComPtr(ComPtr &&other) noexcept : m_ptr(other.m_ptr) { other.m_ptr = nullptr; }
  ComPtr &operator=(ComPtr &&other) noexcept {
    if (this != &other) {
      reset();
      m_ptr = other.m_ptr;
      other.m_ptr = nullptr;
    }
    return *this;
  }
  ~ComPtr() { reset(); }

  T **operator&() {
    reset();
    return &m_ptr;
  }
  T *get() const { return m_ptr; }
  T *operator->() const { return m_ptr; }
  explicit operator bool() const { return m_ptr != nullptr; }

  void reset(T *p = nullptr) {
    if (m_ptr)
      m_ptr->Release();
    m_ptr = p;
  }

  T *detach() {
    T *temp = m_ptr;
    m_ptr = nullptr;
    return temp;
  }

private:
  T *m_ptr{nullptr};
};

} // namespace mediaplayer

#endif // _WIN32

#endif // MEDIAPLAYER_COMPTR_H
