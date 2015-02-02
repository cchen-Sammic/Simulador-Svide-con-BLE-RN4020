#ifndef SVIDE
#define SVIDE
//#define Q_OBJECT

#include <QString>
#include <QObject>

class svideBle: public QObject
{
//    Q_OBJECT

public:
      svideBle(QObject *parent = 0);
     ~svideBle();

      QString characterEstado();
      void characterOrdenCiclo(QByteArray orden);
      QString preConfigBleSammic(int pos_Ble);
public:
    QString UUID_servicio;
    QString UUID_Notificacion;
    QString UUID_EscrituraLectura;

/*BLE caractristica para Estado*/
    QString estado_info;
    int estado_tempAgua;
    int estado_tempSonda;
    int estado_tiempo_ciclo;
    int estado_tiempo_calentamiento;
/*BLE caractristica para Orden*/
    int orden_tempAgua;
    int orden_tempSonda;
    int orden_tiempoCiclo;
    QString label_orden;

};

#endif // SVIDE
