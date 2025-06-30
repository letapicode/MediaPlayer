#ifndef MEDIAPLAYER_PROTOCOLSUPPORT_H
#define MEDIAPLAYER_PROTOCOLSUPPORT_H

#include <string>
#include <vector>

namespace mediaplayer {

std::vector<std::string> inputProtocols();
bool isInputProtocolSupported(const std::string &protocol);

} // namespace mediaplayer

#endif // MEDIAPLAYER_PROTOCOLSUPPORT_H
