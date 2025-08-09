#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QPushButton>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMessageBox>
#include <QMouseEvent>
#include <QListWidget>
#include <random>
#include <QDir>
#include <QRandomGenerator>
#include <QFileInfoList>
#include <QPixmap>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void reproducir();
    void addSong();
    void pasar();
    void atras();
    void actualizarPosicion(qint64 posicion);
    void actualizarDuracion(qint64 duracion);
    void guardarCanciones();
    void cargarCanciones();
    void existe();
    void checkEnd(QMediaPlayer::MediaStatus estado);
    void remove();
    void actualizarCanciones();
    QString actualizarTiempo(qint64 ms);
    void cambiarPosicion(int pos);
    void mostrarCanciones();
    void mousePressEvent(QMouseEvent *event);
    void reproducirCanciones(QListWidgetItem *item);
    void randomCheck();
    void loopCheck();
    void loadPathCovers();
    void showRandomCover();

private:
    Ui::MainWindow *ui; // Ventana principal
    QMediaPlayer *reproductor; // Usado para reproducir la música
    QVector<QUrl> playlist; // Donde guardaremos todas las canciones
    QAudioOutput *salidaAudio; // Se usa para poder sacar audio
    QStringList coverPaths;
    QRandomGenerator *rng;
    int posicion; // Controla la posicion del array
    bool random; // Controla el modo random
    bool loop; // Comprueba si se repite la canción en bucle

};

#endif // MAINWINDOW_H
