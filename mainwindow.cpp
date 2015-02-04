#include "mainwindow.h"
#include "svideBle.h"
#include "ui_mainwindow.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QtSerialPort>
#include <QQuickItem>

#include <QDebug>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Svide = new svideBle();
    timer_BLEconfig = new QTimer(this);
    timer_BLEnotificacion = new QTimer(this);
    timer_termocirculador = new QTimer(this);
    value_dialMin_set = ui->dial_min->value();
    value_dialTemp_set = ui->dial_temp->value();
    estadoSvide = "reposo";
    dataSerial = "";
    min_actual = 0;
    segRestante_actual = 0;
    temp_aguaInicial = 20;
    temp_actual = temp_aguaInicial;
//    temp_actual = 20;
    contarTiempoMostrar=0; ///para enviar tiempo actual al GUI
    comandoFinalizado=false;

    ui->button_blenotify->setDefault(false);
    Ble_servicio = Svide->UUID_servicio;
    Ble_notificacion = Svide->UUID_Notificacion;
    Ble_escLect = Svide->UUID_EscrituraLectura;
    ui->show_UUID->insertPlainText("R/W: "+Ble_escLect+"\n");
    ui->show_UUID->insertPlainText("Notif: "+Ble_notificacion);

    numComandoIntro =1;  //COMANDOS   introducidos
    numBLEConfig =0;  //CONTADOR  para configuración BLE Servicios/Características Sammic
    //    ui->button_bleconfig->setEnabled(false);

    QUrl source("qrc:userInterface/gui_test.qml");
    ui->guiSVide->setSource(source);
    m_currentRootObject = ui->guiSVide->rootObject();
    //    qDebug()<<m_currentRootObject;

    serial = new QSerialPort(this);
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()){
        ui->portBox->addItem(info.portName());
        //        qDebug()<<info.portName();
    }
    ui->portBox->setCurrentIndex(ui->portBox->count()-1); //ui->portBox->count()-1

    ui->baudBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->baudBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->baudBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->baudBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    ui->baudBox->setCurrentIndex(3);

    initActionsConnections();
}


MainWindow::~MainWindow()
{
    delete ui;
}
//![1] Connections
void MainWindow :: initActionsConnections(){
    //comunicacion serie
    connect(ui->serialComButton, SIGNAL(clicked()), this, SLOT(openSerialPort()));
    connect(serial, SIGNAL(readyRead()),this, SLOT(onSerialRead()));
    connect(ui->comando, SIGNAL(returnPressed()), SLOT(onSerialWrite()));
    //widget
    connect(ui->dial_min, SIGNAL(valueChanged(int)), this, SLOT(valueChangedDialMin()));
    connect(ui->dial_temp, SIGNAL(valueChanged(int)), this, SLOT(valueChangedDialTemp()));
    connect(ui->button_svideStart,SIGNAL(clicked()),this,SLOT(onStartBotonTermocirculador()));
    //BLE
    connect(ui->button_bleconfig,SIGNAL(clicked()),this, SLOT(initBLEconfig()));
    connect(timer_BLEconfig, SIGNAL(timeout()), this, SLOT(update_initBLEconfig()));
    timer_BLEconfig->stop();
    connect(ui->button_blenotify,SIGNAL(clicked()),this, SLOT(BLEnotify()));
    connect(timer_BLEnotificacion,SIGNAL(timeout()),this,SLOT(update_BLEnotify()));
    timer_BLEnotificacion->stop();
    //termocirculador
    connect(timer_termocirculador, SIGNAL(timeout()),this, SLOT(update_termocirculador()));
    timer_termocirculador->stop();
    connect(this,SIGNAL(ordenCicloSvide()),this,SLOT(valueChangedDialTemp()));
    connect(this,SIGNAL(ordenCicloSvide()),this, SLOT(valueChangedDialMin()));
}
//![2] Serial Port
void MainWindow :: openSerialPort()
{    // SET SERIAL COMMUNICATION
    if(!serial->isOpen()){
        serial->setPortName(ui->portBox->currentText());
        serial->setBaudRate(ui->baudBox->currentData().toInt());
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity( QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setFlowControl( QSerialPort::NoFlowControl);
        serial->open(QIODevice::ReadWrite);

        qDebug()<<"conexión:"<<serial->portName()<<" bps:"<<serial->baudRate()<<"databits:"<<serial->dataBits()
               <<"paridad:"<<serial->parity()<<" stopbit:"<<serial->stopBits()<<"flow:"<<serial->flowControl();

        if(serial->isOpen()){
            ui->serialComButton->setIcon(QIcon(":/iconos/imagenes/connect.png"));
            timer_termocirculador->start(2000);
        }
        ui->button_bleconfig->setEnabled(true);
    }
    else{
        serial->close();
        ui->serialComButton->setIcon(QIcon(":/iconos/imagenes/disconnect.png"));
    }
}
//![3] Serial Read
void MainWindow :: onSerialRead(){
    if (serial->bytesAvailable()) {//bytesAvailable
        ui->consola->setTextColor("grey");
        QByteArray data;
            data = serial->readAll();
            dataSerial.append(data);
        if(dataSerial.contains("#")){
            ui->consola->insertPlainText(QString(dataSerial));
            qDebug()<<dataSerial;
//            qDebug()<<"longitud dato:"<<dataSerial.length();
            ui->consola->moveCursor(QTextCursor::End);
            ui->consola->setTextColor("black");
            if(dataSerial.contains("WV")){
                QByteArray dataOrden = dataSerial.mid(8,10);
                //ENVIO DE ORDEN A SVIDE
                Svide->characterOrdenCiclo(dataOrden);
                if(estadoSvide=="reposo"){
                    ui->dial_min->setValue(Svide->orden_tiempoCiclo);
                    ui->dial_temp->setValue(Svide->orden_tempAgua);
                    ui->label_ordenCiclo->setText(Svide->label_orden);
                    emit ordenCicloSvide();
                    ui->button_svideStart->clicked();

                }
            }
            dataSerial=""; //BORRAR  VALORES  VECTOR   DATASERIAL
        }
    }
}
//![4] Serial Write
void MainWindow::onSerialWrite(){
    if (serial->isOpen() && !ui->comando->text().isEmpty()){
        QString writeDatos = QString("\n[%1").arg(numComandoIntro)+"]: "+QString(ui->comando->text())+"\n";
        ui->consola->insertPlainText(writeDatos);
        serial->write(QByteArray(ui->comando->text().toLatin1()));//toLocal8Bit
        //        qDebug()<<writeDatos;
        ui->comando->clear();
        numComandoIntro ++;
    }
}
//![5] Dial Minuto
void MainWindow::valueChangedDialMin(){
    value_dialMin_set = ui->dial_min->value();
    QString tiempoSet = SET_tiempoCicloToString(value_dialMin_set);
    m_currentRootObject->setProperty("stringTiemposSet",tiempoSet);
    m_currentRootObject->setProperty("angTiempo_p", value_dialMin_set);
}
//![6] Dial Temperatura
void MainWindow::valueChangedDialTemp(){
    value_dialTemp_set = ui->dial_temp->value();
    m_currentRootObject->setProperty("angTemp_p", value_dialTemp_set);
}
//![7] preConfiguración BLE SAMMIC
void MainWindow::initBLEconfig(){
    ui->button_bleconfig->setStyleSheet("background-color:rgb(168, 255, 53);");
    ui->consola->setTextColor("blue");
    ui->consola->insertPlainText("** Iniciando configuración RN4020 de Sammic: **\n");
    timer_BLEconfig->start(2000);
}
//![7] update BLE config
void MainWindow::update_initBLEconfig(){
    ui->consola->setTextColor("blue");
    QString comando;
    numBLEConfig++;
    comando = Svide->preConfigBleSammic(numBLEConfig);

    if(comandoFinalizado==false){
        serial->write(comando.toLatin1());
        comando = "-> "+comando+"\n";
        qDebug()<<comando;
        ui->consola->insertPlainText(comando);
        if (comando=="-> R,1\n"){
            comandoFinalizado =true;
        }
    }
    else if(comandoFinalizado==true){
        comando ="** Configuración finalizada ** ¡reiniciar RN4020!";
        comando =comando+"\n";
        qDebug()<<comando;
        ui->consola->insertPlainText(comando);
        ui->button_bleconfig->setStyleSheet("background-color: rgb(181, 161, 181);");
        ui->consola->setTextColor("black");
        comandoFinalizado=false;
        numBLEConfig=0;
        timer_BLEconfig->stop();
    }
}
//![8] Envio de notificación BLE SAMMIC
void MainWindow::BLEnotify(){
    if(!ui->button_blenotify->isDefault()){
        ui->button_blenotify->setStyleSheet("background-color:rgb(255, 178, 102);");
        ui->button_blenotify->setDefault(true);
        //        qDebug()<<ui->button_blenotify->isDefault();
        timer_BLEnotificacion->start(2000);
    }
    else if(ui->button_blenotify->isDefault()){
        ui->button_blenotify->setStyleSheet("");
        ui->button_blenotify->setDefault(false);
        //        qDebug()<<ui->button_blenotify->isDefault();
        timer_BLEnotificacion->stop();
        ui->label_notificacion->setText("notificacion: Cancelada");
    }
    //    qDebug()<<ui->button_blenotify->isDefault();
}
//![8] update Notificación BLE
void MainWindow::update_BLEnotify(){
    QString soloUUIDnotify = Ble_notificacion.left(32);
    //    qDebug()<<soloUUIDnotify;
    QString dato_notificacion = Svide->characterEstado();
    QString notificacion ="suw,"+soloUUIDnotify+","+dato_notificacion;
    qDebug()<<notificacion;
    ui->label_notificacion->setText("notificacion:"+dato_notificacion);
    serial->write(notificacion.toLatin1());

}
//![9] Funcionamiento Svide
void MainWindow::onStartBotonTermocirculador(){
    if(estadoSvide=="reposo"){
        //        value_dialTemp_set = ui->dial_temp->value();
        estadoSvide="calentando";
        timer_termocirculador->start(150);
        imagenMostrarAgua =true;
        //         m_currentRootObject->setProperty("onCiclo",false);
    }
    else if(estadoSvide=="calentamientoTerminado"){
        estadoSvide="ciclo";
        ui->label_imagIntro->setPixmap(QPixmap(":/iconos/imagenes/agua3_ciclo.png"));
        timer_termocirculador->start(200);
        m_currentRootObject->setProperty("tiempo_mostrar",1);
    }
    else if(estadoSvide=="ciclo"){
        estadoSvide="pausa";
        ui->label_imagIntro->setPixmap(QPixmap(":/iconos/imagenes/agua3_pausa.png"));
    }
    else if(estadoSvide=="pausa"){
        estadoSvide="ciclo";
        ui->label_imagIntro->setPixmap(QPixmap(":/iconos/imagenes/agua3_ciclo.png"));
        timer_termocirculador->start(200);
        m_currentRootObject->setProperty("tiempo_mostrar",1);
    }
    else if(estadoSvide=="completado"){
        estadoSvide="reposo";
        m_currentRootObject->setProperty("tiempo_mostrar",0);
        ui->label_imagIntro->setPixmap(QPixmap(":/iconos/imagenes/agua00_reposo.png"));
        Svide->estado_info= estadoSvide;
        ui->label_ordenCiclo->setText("Orden para Svide\n\nagua set: ---\nsonda set: ---\ntiempo set: ---");
        temp_actual = temp_aguaInicial;
        // temp_actual = 20;
        min_actual = 0;
        ui->label_ordenCiclo->setText("Orden para Svide\n\nagua set: ---\nsonda set: ---\ntiempo set: ---");

    }
}
//![9] update Svide
void MainWindow::update_termocirculador(){
    //    if(serial->isOpen()){
    if(estadoSvide=="reposo"){
    }
    else if(estadoSvide =="calentando"){
        temp_actual = temp_actual + 0.2; //ACTUALIZACION   TEMP
        segRestante_actual = (value_dialTemp_set-temp_actual);//(timer_termocirculador->interval())/1000*(value_dialTemp_set-temp_actual);//ACTUALIZACION TIEMPO RESTANTE CALENTAMIENTO
        //ALTERNAR   IMAGEN agua0 calentando
        if(imagenMostrarAgua){
            ui->label_imagIntro->setPixmap(QPixmap(":/iconos/imagenes/agua0.png"));
            imagenMostrarAgua=false;
        }
        //ALTERNAR   IMAGEN agua1 calentando
        else if(!imagenMostrarAgua){
            ui->label_imagIntro->setPixmap(QPixmap(":/iconos/imagenes/agua1.png"));
            imagenMostrarAgua=true;
        }
        //ALTERNAR   IMAGEN  agua2 calentamiento terminado
        if(temp_actual>=value_dialTemp_set){
            temp_actual=value_dialTemp_set;
            estadoSvide="calentamientoTerminado";
            ui->label_imagIntro->setPixmap(QPixmap(":/iconos/imagenes/agua2.png"));
            timer_termocirculador->stop();
        }
        QString stringTiempoCalentamiento =tiempoCalentamientoToString(segRestante_actual);
        m_currentRootObject->setProperty("stringTiempo",stringTiempoCalentamiento);
        float porcentTemp = float(temp_actual-temp_aguaInicial)/float(value_dialTemp_set-temp_aguaInicial);
        QString stringTemp =QString::number(temp_actual,'f',2);
        m_currentRootObject->setProperty("porcentTemperatura",porcentTemp);
        m_currentRootObject->setProperty("stringTempActual",stringTemp);
        m_currentRootObject->setProperty("temp_mostrar", temp_actual);

    }
    else if(estadoSvide =="ciclo"){
        if(contarTiempoMostrar == 0){
            min_actual = min_actual + 0.2;
            contarTiempoMostrar = contarTiempoMostrar+1;
            ui->label_imagIntro->setPixmap(QPixmap(":/iconos/imagenes/agua3_ciclo.png"));
            if(min_actual>= value_dialMin_set){ //COMPLETADO
                min_actual =  ui->dial_min->value();
                estadoSvide ="completado";
                ui->label_imagIntro->setPixmap(QPixmap(":/iconos/imagenes/agua4_fin.png"));
                timer_termocirculador->stop();
            }
        }
        else if(contarTiempoMostrar!=0){
            min_actual = min_actual + 0.2;
            contarTiempoMostrar = contarTiempoMostrar +1;
            ui->label_imagIntro->setPixmap(QPixmap(":/iconos/imagenes/agua3_ciclo2.png"));
            if(min_actual>= value_dialMin_set){ //COMPLETADO
                min_actual =  ui->dial_min->value();
                estadoSvide ="completado";
                ui->label_imagIntro->setPixmap(QPixmap(":/iconos/imagenes/agua4_fin.png"));
                timer_termocirculador->stop();
            }
            if(contarTiempoMostrar==10){ //TIEMPO   SEGUNDOS   MUERTOS
                contarTiempoMostrar=0;
            }
        }
        QString conversionTiempoCiclo = tiempoCicloToString(min_actual);
        m_currentRootObject->setProperty("stringTiempo",conversionTiempoCiclo);
        float porcenttiempo = min_actual/value_dialMin_set;
        m_currentRootObject->setProperty("porcenttiempo",porcenttiempo);
    }
    else if(estadoSvide=="pausa"){
        timer_termocirculador->stop();
    }

    Svide->estado_info= estadoSvide;
    Svide->estado_tempAgua = temp_actual;
    Svide->estado_tiempo_ciclo = min_actual;
    Svide->estado_tiempo_calentamiento = segRestante_actual;

}
//![10] Conversion segundos de calentamiento a formato 00' 00"
QString MainWindow::tiempoCalentamientoToString(float tiempoRestante){
   float conversionTiempo = tiempoRestante * 5;  // TIEMPO FICTICIO
   int min = int(conversionTiempo)/60;
   int seg = int(conversionTiempo)%60;
   QString stringMin = QString::number(min);
   QString stringSeg = QString::number(seg);
   if(min<10){
       stringMin= "0" + stringMin;
   }
   if(seg<10){
       stringSeg = "0" + stringSeg;
   }
   QString composicionTiempo = stringMin+"' "+stringSeg+"''";
   return composicionTiempo;
}
//![11] Conversion minutos set ciclo a formato 0h 00'
QString MainWindow::SET_tiempoCicloToString(int tiempoSet){
    int hora = tiempoSet/60;
    int min = tiempoSet%60;
    QString stringHora = QString::number(hora);
    QString stringMin = QString::number(min);
//    if(hora<10){
//        stringHora= "0" + stringHora;
//    }
    if(min<10){
        stringMin = "0" + stringMin;
    }
    QString composicionTiempo = stringHora+"h "+stringMin+"'";
    return composicionTiempo;
}

//![12] Conversion tiempo ciclo a formato 0h 00' 00"
QString MainWindow::tiempoCicloToString(float tiempo){
    int hora = tiempo/60;
    int min = int(tiempo)%60;
    int seg = (tiempo - int(tiempo))*60;
    QString stringHora = QString::number(hora);
    QString stringMin = QString::number(min);
    QString stringSeg = QString::number(seg);
    if(min<10){
        stringMin= "0" + stringMin;
    }
    if(seg<10){
        stringSeg = "0" + stringSeg;
    }
    QString composicionTiempo = stringHora+"h "+stringMin+"' " + stringSeg+"''";
    return composicionTiempo;
}
