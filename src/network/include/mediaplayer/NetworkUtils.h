#ifndef MEDIAPLAYER_NETWORKUTILS_H
#define MEDIAPLAYER_NETWORKUTILS_H

#include <string>
#include <vector>

namespace mediaplayer {

std::vector<std::string> listProtocols(bool output = false);
bool supportsProtocol(const std::string &protocol);
bool supportsDemuxer(const std::string &name);

} // namespace mediaplayer

#endif // MEDIAPLAYER_NETWORKUTILS_H
