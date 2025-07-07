#include "mediaplayer/VoskRecognizer.h"
#include "mediaplayer/VoiceInputQt.h"
#include <vosk_api.h>

using namespace mediaplayer;

VoskRecognizer::VoskRecognizer(const QString &modelPath, const std::vector<QString> &grammar,
                               QObject *parent)
    : QObject(parent) {
  vosk_set_log_level(0);
  m_model = vosk_model_new(modelPath.toUtf8().constData());
  std::string json = "[";
  for (size_t i = 0; i < grammar.size(); ++i) {
    json += '"' + grammar[i].toStdString() + '"';
    if (i + 1 < grammar.size())
      json += ',';
  }
  json += ']';
  m_rec = vosk_recognizer_new_grm(m_model, 16000.0f, json.c_str());
  m_input = new VoiceInputQt(this, this);
}

VoskRecognizer::~VoskRecognizer() {
  stopListening();
  if (m_rec)
    vosk_recognizer_free(m_rec);
  if (m_model)
    vosk_model_free(m_model);
}

void VoskRecognizer::startListening() {
  if (m_active.load())
    return;
  m_active = true;
  if (m_input)
    m_input->start();
}

void VoskRecognizer::stopListening() {
  if (!m_active.load())
    return;
  m_active = false;
  if (m_input)
    m_input->stop();
}

void VoskRecognizer::feedAudio(const int16_t *data, int length) {
  if (!m_rec || !m_active.load())
    return;
  vosk_recognizer_accept_waveform(m_rec, reinterpret_cast<const char *>(data),
                                  length * sizeof(int16_t));
  const char *result = vosk_recognizer_partial_result(m_rec);
  if (result && *result) {
    QString res = QString::fromUtf8(result);
    emit commandRecognized(res);
  }
}
