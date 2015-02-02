#include "svideBle.h"
#include <qdebug.h>

svideBle::svideBle (QObject *parent)
    : QObject(parent)
{
    UUID_servicio ="53346d6d3163424c4573337276316365";//Set private service UUID 'S4mm1cBLEs3rv1ce'
    UUID_EscrituraLectura="53346d6d3163424c45657363726c6563,0A,05";//0A:Read/Write 'S4mm1cBLEescrlec'
    UUID_Notificacion="53346d6d3163424c456e6f7479666963,10,06";//10:Notify 'S4mm1cBLEnotyfic'
    estado_info = "reposo";

    orden_tempAgua=0;
    orden_tempSonda=0;
    orden_tiempoCiclo=0;
}
svideBle::~svideBle(){

}

QString svideBle::characterEstado(){
    QString return_var;
    QString var1;
    if(true){//ESTADO  SVIDE
        if(estado_info=="reposo"){
            var1="0";
        }
        else if(estado_info=="calentando"){
            var1="1";
        }
        else if(estado_info=="calentamientoTerminado"){
            var1="2";
        }
        else if(estado_info=="ciclo"){
            var1="3";
        }
        else if(estado_info=="pausa"){
            var1="4";
        }
        else if(estado_info=="completado"){
            var1="5";
        }
    }
    QString var2;
    var2.append(QString("%1").arg(estado_tempAgua*100)); //TEMPERATU RA  agua
    QString var3 = "0000"; // TEMPERATURA  sonda
    QString var4 ;
    if(estado_info=="calentando") var4=(QString("%1").arg(estado_tiempo_calentamiento,3,16)).replace(QString(" "),QString("0"));
    else if(estado_info=="ciclo" || estado_info=="pausa") var4= (QString("%1").arg(estado_tiempo_ciclo,3,16)).replace(QString(" "),QString("0"));

    if(var1=="2" || var1=="5" || var1=="0"){
        var4="000";
    }
    return return_var = var1+var2+var3+var4;
}

void svideBle::characterOrdenCiclo(QByteArray orden){
    QByteArray tipo = orden.mid(0,1);
    qDebug()<<"tipo"<<tipo;
    QByteArray tempAgua = orden.mid(1,3);
    qDebug()<<"temp agua"<<tempAgua;
    QByteArray tempSonda = orden.mid(4,3);
    qDebug()<<"temp sonda"<<tempSonda;
    QByteArray tiempoCiclo = orden.mid(7,3);
    qDebug()<<"tiempo ciclo"<<tiempoCiclo;
    if(tipo=="0"){
        orden_tempAgua= int((tempAgua.toInt())/10);
        orden_tempSonda = int((tempSonda.toInt())/10);
        orden_tiempoCiclo = tiempoCiclo.toInt();
        qDebug()<<orden_tempAgua<<" "<<orden_tempSonda<<" "<<orden_tiempoCiclo;
        label_orden = QString("Orden para Svide\n\nagua set: %1\nsonda set: %2\ntiempo set: %3")
                .arg(orden_tempAgua).arg(orden_tempSonda).arg(orden_tiempoCiclo);
    }
}

QString svideBle::preConfigBleSammic(int pos_Ble){
    QString _comando;
    switch(pos_Ble){
    case 1:
        _comando = "SF,1"; //factory reset
        break;
//    case 2:
//        _comando ="SR,20000000";//Auto advertise
//        break;
    case 3:
        _comando ="SR,04000000";//Not direct advertisement
        break;
    case 4:
        _comando = "SS,00000001"; //Enable private service support
        break;
    case 5:
        _comando = "PZ"; //clear the current private service and characteristics
        break;
    case 6:
        _comando = "PS,"+UUID_servicio; //Set private service UUID
        break;
    case 7:
        _comando = "PC,"+ UUID_EscrituraLectura;//53346d6d3163424c45657363726c6563,0A,0F"; //Read/Write
        break;
    case 8:
        _comando = "PC,"+UUID_Notificacion;//53346d6d3163424c456e6f7479666963,10,02";//Notify
        break;
    case 9:
        _comando ="R,1"; //Reboot
        break;
    default:
        break;
    }
    return _comando;
}


