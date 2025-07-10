#include <QDebug>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <chrono>

int main(int argc, char *argv[]) {
  auto startupBegin = std::chrono::steady_clock::now();
  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;
  const QUrl url = QUrl::fromLocalFile("qml/Main.qml");
  engine.load(url);
  if (engine.rootObjects().isEmpty())
    return -1;
  auto startupEnd = std::chrono::steady_clock::now();
  double startupSeconds = std::chrono::duration<double>(startupEnd - startupBegin).count();
  qInfo() << "Startup completed in" << startupSeconds << "seconds";
  if (!qEnvironmentVariableIsEmpty("MEASURE_STARTUP"))
    return 0;
  return app.exec();
}
