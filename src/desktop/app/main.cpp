#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;
  const QUrl url = QUrl::fromLocalFile("qml/Main.qml");
  engine.load(url);
  if (engine.rootObjects().isEmpty())
    return -1;
  return app.exec();
}
