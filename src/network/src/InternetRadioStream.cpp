#include "mediaplayer/InternetRadioStream.h"
#include <iostream>
#include <libavformat/avformat.h>

namespace mediaplayer {

bool InternetRadioStream::open(const std::string &url) {
  if (!NetworkStream::open(url))
    return false;
  return refreshMetadata();
}

static std::string getTag(AVFormatContext *ctx, const char *key) {
  AVDictionaryEntry *tag = av_dict_get(ctx->metadata, key, nullptr, 0);
  return tag && tag->value ? tag->value : std::string();
}

bool InternetRadioStream::refreshMetadata() {
  AVFormatContext *ctx = context();
  if (!ctx)
    return false;
  m_metadata.name = getTag(ctx, "icy-name");
  m_metadata.genre = getTag(ctx, "icy-genre");
  m_metadata.url = getTag(ctx, "icy-url");
  m_metadata.title = getTag(ctx, "icy-title");
  m_metadata.description = getTag(ctx, "icy-description");
  return true;
}

} // namespace mediaplayer
