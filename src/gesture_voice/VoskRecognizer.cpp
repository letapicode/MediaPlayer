#include "VoskRecognizer.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QStandardPaths>
#include <vosk_api.h>

using namespace mediaplayer;

struct VoskRecognizer::Impl {
  VoskModel *model{nullptr};
  VoskRecognizer *rec{nullptr};
  QByteArray grammar;
  bool running{false};
};

VoskRecognizer::VoskRecognizer(QObject *parent) : QObject(parent), m(new Impl) { loadGrammar(); }

void VoskRecognizer::loadGrammar() {
  QFile f(":/gesture_voice/commands.json");
  if (!f.open(QIODevice::ReadOnly))
    return;
  m->grammar = f.readAll();
}

bool VoskRecognizer::loadModel(const QString &modelPath) {
  if (m->model)
    vosk_model_free(m->model);
  m->model = vosk_model_new(modelPath.toUtf8().constData());
  if (!m->model)
    emit error(tr("Failed to load model"));
  return m->model != nullptr;
}

bool VoskRecognizer::start() {
  if (!m->model) {
    emit error(tr("Model not loaded"));
    return false;
  }
  if (m->rec)
    vosk_recognizer_free(m->rec);
  const char *grammarStr = m->grammar.isEmpty() ? nullptr : m->grammar.constData();
  m->rec = vosk_recognizer_new_grm(m->model, 16000.0f, grammarStr);
  m->running = m->rec != nullptr;
  if (m->running)
    emit runningChanged(true);
  else
    emit error(tr("Failed to start recognizer"));
  return m->running;
}

void VoskRecognizer::stop() {
  if (m->rec) {
    vosk_recognizer_free(m->rec);
    m->rec = nullptr;
    m->running = false;
    emit runningChanged(false);
  }
}

void VoskRecognizer::feedAudio(const QByteArray &data) {
  if (!m->rec) {
    emit error(tr("Recognizer not running"));
    return;
  }
  int r = vosk_recognizer_accept_waveform(m->rec, (const char *)data.constData(), data.size());
  if (r) {
    const char *json = vosk_recognizer_result(m->rec);
    QJsonDocument doc = QJsonDocument::fromJson(QByteArray(json));
    QString text = doc.object().value("text").toString();
    if (!text.isEmpty())
      emit textRecognized(text);
  }
}

bool VoskRecognizer::isRunning() const { return m->running; }
