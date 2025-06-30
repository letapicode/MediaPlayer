#include "mediaplayer/InternetRadio.h"

extern "C" {
#include <libavformat/avformat.h>
}

namespace mediaplayer {

bool InternetRadio::open(const std::string &url) {
  AVDictionary *opts = nullptr;
  av_dict_set(&opts, "icy", "1", 0);
  bool ok = NetworkStream::open(url, opts);
  av_dict_free(&opts);
  if (!ok)
    return false;
  if (avformat_find_stream_info(context(), nullptr) < 0) {
    return false;
  }
  return true;
}

std::string InternetRadio::metaValue(const std::string &key) const {
  if (!context())
    return {};
  AVDictionaryEntry *e = av_dict_get(context()->metadata, key.c_str(), nullptr, 0);
  if (e && e->value)
    return e->value;
  return {};
}

std::string InternetRadio::currentTitle() const {
  std::string title = metaValue("icy-title");
  if (title.empty())
    title = metaValue("StreamTitle");
  return title;
}

} // namespace mediaplayer
