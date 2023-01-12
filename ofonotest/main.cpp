#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QWindow>

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QLatin1String("/opt/examples/libqofono-qt5/qml/ofonotest/main.qml")));

    return app.exec();
}
