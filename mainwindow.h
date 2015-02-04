#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "svideBle.h"
#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QQuickWidget>
#include <QQuickView>
#include <QTimer>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    svideBle *Svide;
    QSerialPort *serial;
    QString datosSerial;
    QByteArray dataSerial;
    QObject *m_currentRootObject;
    QString text ;
    QString inString;
    int numComandoIntro;
    int numBLEConfig;
    QTimer *timer_BLEconfig;
    QString Ble_servicio;
    QString Ble_notificacion;
    QString Ble_escLect;
    QTimer *timer_BLEnotificacion;
    QTimer *timer_termocirculador;
    QString estadoSvide;
    int value_dialMin_set;
    int value_dialTemp_set;
    float min_actual;
    int temp_aguaInicial;
    float temp_actual;
    float segRestante_actual;
    bool imagenMostrarAgua;
    int contarTiempoMostrar;
    bool comandoFinalizado;


private slots:
    void openSerialPort();
    void onSerialRead();
    void onSerialWrite();

    void valueChangedDialTemp();
    void valueChangedDialMin();
    void onStartBotonTermocirculador();
    void initBLEconfig();
    void update_initBLEconfig();
    void BLEnotify();
    void update_BLEnotify();
    void update_termocirculador();

private:
    void initActionsConnections();
    QString tiempoCalentamientoToString(float tiempoRestante);
    QString SET_tiempoCicloToString(int tiempoSet);
    QString tiempoCicloToString(float tiempo);

signals:
    void ordenCicloSvide();



};
#endif // MAINWINDOW_H
