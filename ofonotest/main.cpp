#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QWindow>

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QLatin1String("/usr/share/libqofono-qt5/examples/qml/ofonotest/main.qml")));

    return app.exec();
}
