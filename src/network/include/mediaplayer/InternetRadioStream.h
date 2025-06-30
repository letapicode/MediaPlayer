#ifndef MEDIAPLAYER_INTERNETRADIOSTREAM_H
#define MEDIAPLAYER_INTERNETRADIOSTREAM_H

#include "NetworkStream.h"
#include <string>

namespace mediaplayer {

struct IcyMetadata {
  std::string name;
  std::string genre;
  std::string url;
  std::string title;
  std::string description;
};

class InternetRadioStream : public NetworkStream {
public:
  bool open(const std::string &url);
  const IcyMetadata &metadata() const { return m_metadata; }
  bool refreshMetadata();

private:
  IcyMetadata m_metadata;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_INTERNETRADIOSTREAM_H
