#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QPushButton>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QListWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/imagenes/imagenes/icon.png"));

    this->setFixedSize(700,550); // Cosas para la ventana y el fondo


    this->setStyleSheet(
        "QMainWindow {"
        "   background-image: url(:/imagenes/imagenes/jpeg.jpg);"
        "   background-repeat: no-repeat;"
        "   background-position: center;"
        "}"
    );

    loadPathCovers(); // Cargamos las imagenes
    showRandomCover(); // Para iniciar una imagen

    reproductor = new QMediaPlayer(this);
    salidaAudio = new QAudioOutput(this);
    reproductor->setAudioOutput(salidaAudio);
    posicion = 0;
    random = false;
    loop = false;
    ui->listaCanciones->hide();
    ui->aux->setFlat(true); // Este es un botón transparente auxiliar para poder mostrar las canciones
    ui->aux->setStyleSheet("background-color: transparent; border: none;");
    this->setWindowTitle("NiPlayer");
    ui->play->setCheckable(true);
    ui->aleatorio->setCheckable(true);
    ui->loop->setCheckable(true);
    ui->listaCanciones->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listaCanciones->setTextElideMode(Qt::ElideRight);

    cargarCanciones();

    connect(ui->play, &QPushButton::clicked, this, &MainWindow::reproducir);
    connect(ui->adelante, &QPushButton::clicked, this, &MainWindow::pasar);
    connect(ui->atras, &QPushButton::clicked, this, &MainWindow::atras);
    connect(ui->addSong, &QPushButton::clicked, this, &MainWindow::addSong);
    connect(ui->quitar, &QPushButton::clicked, this, &MainWindow::remove);
    connect(reproductor, &QMediaPlayer::durationChanged, this, &MainWindow::actualizarDuracion);    // Esto cambia la duración máxima de la canción UNA VEZ
    connect(reproductor, &QMediaPlayer::positionChanged, this, &MainWindow::actualizarPosicion);    // Esto actualiza constantemente el slider
    connect(qApp, &QApplication::aboutToQuit, this, &MainWindow::guardarCanciones);                 // Esto guarda las canciones cuando se cierre la ventana
    connect(reproductor, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::checkEnd);           // Esto pasa la canción al terminar
    connect(ui->progreso, &QSlider::sliderMoved, this, &MainWindow::cambiarPosicion);               // Esto cambia la posición del slider al arrastrar
    connect(ui->aux, &QPushButton::clicked, this, &MainWindow::mostrarCanciones);                   // Muestra las canciones
    connect(ui->listaCanciones, &QListWidget::itemClicked, this, &MainWindow::reproducirCanciones); // Hace que puedas pulsar la cancion de la lista para reproducirla
    connect(ui->aleatorio, &QPushButton::clicked, this, &MainWindow::randomCheck);                  // Cambia el estado de random
    connect(ui->loop, &QPushButton::clicked, this, &MainWindow::loopCheck);



    // Esto lo hacemos para iniciar las etiquetas aunque sea una chapuza


    ui->tActual->setText("0:00");
    for(int i = 0; i < playlist.size(); i++){
        ui->listaCanciones->addItem(playlist[i].fileName());
    }

    if(!playlist.isEmpty()){
        reproductor->setSource(playlist[posicion]);
        ui->nombreCancion->setText(playlist[posicion].fileName());
        reproductor->play();
        reproductor->pause();
    }
    else
        ui->nombreCancion->setText("Ninguna canción seleccionada");
    actualizarCanciones();
}
MainWindow::~MainWindow()
{
    delete ui;
    delete reproductor;
    delete salidaAudio;
}
void MainWindow::addSong(){

    QString filePath;
    QUrl nueva;
    bool insert = true;


    filePath = QFileDialog::getOpenFileName(this, "Selecciona un MP3", "", "Archivos MP3 (*.mp3)");

    //Comprobar que no se pueda añadir la misma canción dos veces

    if(!filePath.isEmpty()){
        nueva = QUrl::fromLocalFile(filePath);

        for(int i = 0; i < playlist.size(); i++){
            if(playlist[i] == nueva){
                insert = false;
            }
        }

        if(insert){
             playlist.append(nueva);
            ui->listaCanciones->addItem(playlist[playlist.size()-1].fileName());
        }
    }
    if(playlist.size() == 1){
        ui->nombreCancion->setText(playlist[0].fileName());
        reproductor->setSource(playlist[0]);
        reproductor->play();
        reproductor->pause();
    }
    actualizarCanciones();
}
void MainWindow::pasar(){
    if(playlist.size()>1){

        existe();
        if(!random){

            if(playlist[posicion] == playlist[playlist.size()-1]){
                posicion=0;
                reproductor->setSource(playlist[posicion]);
            }
            else{
                posicion++;
                reproductor->setSource(playlist[posicion]);
            }
            reproductor->play();
            ui->play->setChecked(true);
            ui->nombreCancion->setText(playlist[posicion].fileName());
        }
        else{

            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distrib(0, playlist.size() - 1);
            int randomIndex;

            do{
                randomIndex = distrib(gen);
            }while(randomIndex == posicion);

            posicion = randomIndex;
            reproductor->setSource(playlist[posicion]);
            reproductor->play();
            ui->nombreCancion->setText(playlist[posicion].fileName());
            ui->play->setChecked(true);
        }
        showRandomCover();
    }
}
void MainWindow::atras(){
    if(playlist.size()>1){

        existe();

        if(playlist[posicion] == playlist[0]){
            posicion = playlist.size()-1;
            reproductor->setSource(playlist[posicion]);
        }
        else{
            posicion--;
            reproductor->setSource(playlist[posicion]);
        }
        reproductor->play();
        ui->play->setChecked(true);
        ui->nombreCancion->setText(playlist[posicion].fileName());
        showRandomCover();
    }
}
void MainWindow::reproducir(){

    if(playlist.isEmpty()) {
        ui->play->setChecked(false);
        return;
    }

    if(reproductor->playbackState() == QMediaPlayer::PlayingState){
        reproductor->pause();
    }
    else{

        if(playlist.size() > 0){

            existe();
            reproductor->play();
        }
        else{
            QMessageBox::warning(this, "PlayList Vacía", "No hay ninguna canción cargada");
        }
    }
}
void MainWindow::actualizarDuracion(qint64 duracion){
    ui->progreso->setMaximum(duracion);
    ui->tTotal->setText(actualizarTiempo(duracion));
}
void MainWindow::actualizarPosicion(qint64 posicion){
    ui->progreso->setValue(posicion);
    ui->tActual->setText(actualizarTiempo(posicion));
}
void MainWindow::cargarCanciones(){

    QFile archivo("urls.txt");
    if(archivo.open(QIODevice::ReadOnly | QIODevice::Text)){    // Abre el archivo en solo modo lectura para leer texto
        QTextStream entrada(&archivo);                          // Flujo de entrada/salida
        while(!entrada.atEnd()){                                // Si no ha terminado
            QString linea = entrada.readLine();                 // Hacemos un getLine
            if(!linea.isEmpty()){                               // Si hemos leido algo
                playlist.append(QUrl(linea));                   // Añadimos a la lista un string.toURL
            }
        }
        archivo.close();                                        // Cerramos el archivo
    }
}
void MainWindow::guardarCanciones(){

    QFile archivo("urls.txt");
    if(archivo.open(QIODevice::WriteOnly | QIODevice::Text))    // Abre el archivo solo en modo escritura y le dice que solo escribirá texto
    {
        QTextStream salida(&archivo);                           // Creamos flujo de entrada / salida
        for(int i = 0; i < playlist.size(); i++){               // Hacemos un bucle para recorrer todos los elementos del vector
            salida << playlist[i].toString() << '\n';           // Pasamos cada URL a string para que se pueda meter
        }
        archivo.close();                                        // Cerramos el archivo
    }
}
void MainWindow::existe(){
    if(!QFile::exists(playlist[posicion].toLocalFile())){
        QMessageBox::warning(this, "Archivo no encontrado", "El archivo " + playlist[posicion].fileName() + " no fue encontrado.");
        playlist.removeAt(posicion);
        if(posicion > 0)
            posicion--;
        else
            posicion = 0;
    }
}
void MainWindow::checkEnd(QMediaPlayer::MediaStatus estado){
    if(estado == QMediaPlayer::EndOfMedia){
        if(!loop && playlist.size() != 1)
            pasar();
        else{
            ui->nombreCancion->setText(playlist[posicion].fileName());
            reproductor->setSource(playlist[posicion]);
            reproductor->play();
        }
    }
}
void MainWindow::remove(){

    if(playlist.isEmpty()) return; // Hace que la funcion no haga nada si esta vacia

    // Eliminar de la playlist y del QListWidget
    playlist.removeAt(posicion);
    delete ui->listaCanciones->takeItem(posicion);

    // Manejar caso especial: última canción eliminada
    if(playlist.isEmpty()){
        posicion = 0;  // Resetear posición
        reproductor->stop();
        ui->play->setChecked(false);
        ui->nombreCancion->setText("Ninguna canción seleccionada");
        showRandomCover();
        ui->tTotal->setText("0:00");
    }
    else {
        // Ajustar posición
        if(posicion >= playlist.size())
            posicion = playlist.size() - 1;

        // Reproducir nueva canción en posición actual
        reproductor->setSource(playlist[posicion]);
        if(ui->play->isChecked()) reproductor->play();
        ui->nombreCancion->setText(playlist[posicion].fileName());
        showRandomCover();
    }

    actualizarCanciones();
}
void MainWindow::actualizarCanciones(){
    ui->cancionesCargadas->setText(QString::number(playlist.size()) + " canciones cargadas");

    if(!playlist.empty())
        ui->nombreCancion->setText(playlist[posicion].fileName());
    else
        ui->nombreCancion->setText("Ninguna canción seleccionada");
    showRandomCover();
}
QString MainWindow::actualizarTiempo(qint64 ms){
    int segundos = ms / 1000;
    int minutos = segundos / 60;
    QString result, min, sec;

    segundos = segundos % 60;

    min = QString::number(minutos);
    sec = QString::number(segundos);

    if(segundos < 10){
        sec = "0" + sec;
    }

    result = min + ":" + sec;

    return result;
}
void MainWindow::cambiarPosicion(int pos){
    reproductor->setPosition(pos);
}
void MainWindow::mostrarCanciones(){
    ui->listaCanciones->show();
}
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    // Si la lista está visible y el clic no fue dentro de ella
    if (ui->listaCanciones->isVisible()){
        ui->listaCanciones->hide(); // Oculta la lista
    }

    QMainWindow::mousePressEvent(event); // Propaga el evento por si otros lo usan
}
void MainWindow::reproducirCanciones(QListWidgetItem *item){
    QString nombre = item->text();
    ui->listaCanciones->hide();
    for(int i = 0; i < playlist.size(); i++){
        if(nombre == playlist[i].fileName()){
            posicion = i;
            reproductor->setSource(playlist[i]);
            reproductor->play();
            ui->nombreCancion->setText(playlist[i].fileName());
            ui->play->setChecked(true);
        }
    }
}
void MainWindow::randomCheck(){

    if(random)
        random = false;
    else
        random = true;

}
void MainWindow::loopCheck(){

    if(loop)
        loop = false;
    else
        loop = true;
}
void MainWindow::loadPathCovers(){

    QStringList paths;

    paths << ":/covers/covers/1.jpeg" << ":/covers/covers/2.jpeg" << ":/covers/covers/3.jpeg" << ":/covers/covers/4.jpeg" << ":/covers/covers/5.jpeg" <<
        ":/covers/covers/6.jpeg" << ":/covers/covers/7.jpeg" << ":/covers/covers/8.jpeg" << ":/covers/covers/9.jpeg" << ":/covers/covers/10.jpeg" << ":/covers/covers/11.jpeg" <<
        ":/covers/covers/12.jpeg" << ":/covers/covers/13.jpeg";

    for(int i = 0; i < paths.size(); i++){
        coverPaths.append(paths.at(i));
    }

    rng = QRandomGenerator::global();

}
void MainWindow::showRandomCover(){

    if (coverPaths.isEmpty()) {
        ui->labelCovers->clear();
        return;
    }

    int idx = rng->bounded(coverPaths.size());
    QPixmap pix(coverPaths.at(idx));

    // Escala el pixmap al tamaño máximo de coverLabel manteniendo la proporción
    QSize labelSize = ui->labelCovers->size(); // será 200×200
    QPixmap scaled = pix.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    ui->labelCovers->setAlignment(Qt::AlignCenter);
    ui->labelCovers->setPixmap(scaled);

}

