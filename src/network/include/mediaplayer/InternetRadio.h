#ifndef MEDIAPLAYER_INTERNETRADIO_H
#define MEDIAPLAYER_INTERNETRADIO_H

#include "NetworkStream.h"
#include <string>

namespace mediaplayer {

class InternetRadio : public NetworkStream {
public:
  bool open(const std::string &url);
  std::string metaValue(const std::string &key) const;
  std::string currentTitle() const;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_INTERNETRADIO_H
