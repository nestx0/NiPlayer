#include "mainwindow.h"

#include <QApplication>
#include <QPushButton>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVector>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setWindowIcon(QIcon(":/imagenes/imagenes/icon.png"));

    QFile f(":/estilo/estilos.qss");
    if (f.open(QFile::ReadOnly | QFile::Text)) {
        QString style = f.readAll();
        a.setStyleSheet(style);
        qDebug() << "QSS cargado correctamente.";
    } else {
        qDebug() << "No se pudo cargar el archivo QSS.";
    }

    MainWindow w;
    w.show();

    return a.exec();
}
