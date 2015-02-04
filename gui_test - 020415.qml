import QtQuick 2.2
import QtQuick.Controls 1.1
//import QtQuick.Window 2.0

Rectangle {
    id: page
        property string currentText: "Valor Introducido"
        property int angTemp_p: 25 //set
        property int angTiempo_p:0 //set
        property int temp_mostrar: 20 //actual
        property int tiempo_mostrar: 0 //actual
        property int tiempo_mostrar_seg:0
        property string  stringTiempo



    width: 430; height: 400
    color: "black"

    Canvas {
        id:mycanvas
        anchors.rightMargin: 0
        anchors.bottomMargin: 0
        anchors.leftMargin: 0
        anchors.topMargin: -14
        anchors.fill: parent
        antialiasing: true


        onPaint: {
            var ctx = getContext("2d");
            ctx.reset();
            var centreX = width / 2;
            var centreY = height / 2 -10;
            var pcAngulotemperatura = Math.PI*2*(temp_mostrar-20)/(angTemp_p-20) ;
//             console.log(tiempo_mostrar+" " +angTiempo_p)
//            console.log(angTemp_p)

            if(true){// circulo temperatura
                // circulo temperatura gris
                ctx.beginPath();
                ctx.strokeStyle = 'rgba(45,45, 45,0.6)';
                ctx.lineWidth = 20;
                ctx.arc(centreX, centreY, width / 3+5, 0 , Math.PI *2 , true);
                ctx.stroke();
                // circulo temperatura rojo
                ctx.beginPath();
                ctx.strokeStyle = 'rgb(255, 20, 44)';
//                ctx.translate(0.9,0.9);
                ctx.lineWidth = 22;
                ctx.arc(centreX, centreY, width / 3+5, Math.PI/2*3 , Math.PI/2*3+ pcAngulotemperatura , false);
                ctx.stroke();
                // circulo temperatarura rojo oscuro
                ctx.beginPath();
                ctx.strokeStyle = '#9C1F1F';//'#731A1A';
//                ctx.translate(0.9,0.9);
                ctx.lineWidth = 24;
                ctx.arc(centreX, centreY, width / 3+5, Math.PI/2*3+pcAngulotemperatura-Math.PI/180*3 , Math.PI/2*3+ pcAngulotemperatura+Math.PI/180*1 , false);
                ctx.stroke();
                // circulo temperatarura raya blanca
                ctx.beginPath();
                ctx.strokeStyle = 'black';
                ctx.lineWidth = 20;
                ctx.arc(centreX, centreY, width / 3+5, Math.PI/2*3+pcAngulotemperatura-Math.PI/180*3 , Math.PI/2*3+pcAngulotemperatura-Math.PI/180*2.5  , false);
                ctx.stroke();
            }

            if(true){// circulo tiempo cuenta atrás
                var anguloColor_i =Math.PI/2*3 // orientación inicial
                var anguloColor = Math.PI/180*5.5//* fondo negro
                var anguloColor_f = anguloColor_i + anguloColor
                var angulofondo_i = anguloColor_f
                var angulofondo = Math.PI/180*2.5//* barra color
                var angulofondo_f = angulofondo_i + angulofondo
                var porcentColor
                if(tiempo_mostrar== 0 || angTiempo_p== 0){
                     porcentColor = 0
                    stringTiempo = tiempo_mostrar_seg.toString() +" seg"
                }
                else{
                     porcentColor = Math.PI*2/(anguloColor+angulofondo)*(1-tiempo_mostrar/angTiempo_p)
                    stringTiempo = tiempo_mostrar.toString()+" min"
                }
                for (var i=0; i<Math.PI*2/(anguloColor+angulofondo); i++){
                    ctx.beginPath();
                    ctx.strokeStyle = 'black'
                    ctx.lineWidth = 18;
                    ctx.arc(centreX, centreY,width /3.6, anguloColor_i ,anguloColor_f, false);
                    ctx.stroke();

                    if(i<porcentColor-1){
                        ctx.beginPath();
                        ctx.strokeStyle = 'white'
                        ctx.lineWidth = 18;
                        ctx.arc(centreX, centreY,width /3.6,anguloColor_f,anguloColor_f + angulofondo , false );
                        ctx.stroke();
                    }
                    if(i>porcentColor-1 && i<porcentColor){
                        ctx.beginPath();
                        ctx.strokeStyle = 'yellow'
                        ctx.lineWidth = 18;
                        ctx.arc(centreX, centreY,width /3.6,anguloColor_f,anguloColor_f+angulofondo , false );
                        ctx.stroke();
                    }

                    else if (i>porcentColor-1){
                        ctx.beginPath();
                        ctx.strokeStyle = 'rgb(40,40, 45)' //gris
                        ctx.lineWidth = 18;
                        ctx.arc(centreX, centreY,width /3.6,anguloColor_f,anguloColor_f+angulofondo , false );
                        ctx.stroke();
                    }
                    anguloColor_i = anguloColor_f+angulofondo
                    anguloColor_f = anguloColor_i+anguloColor
                }
            }

            if(true){// línea blanca separatoria
                ctx.beginPath();
                ctx.translate(0.9,0.9);
                ctx.strokeStyle = 'white';
                ctx.lineWidth = 2;
                ctx.moveTo(0,height/10*9);
                ctx.lineTo(width,height/10*9)
                ctx.stroke();
            }
        }

        Timer {
            interval: 1000;
            repeat: true;
            running: true;
            onTriggered: {
                mycanvas.requestPaint ();
            }
        }

        Text {
            id: text_temperatura_set
            x: 14
            y: 383
            width: 95
            height: 50
            color: "#bd0f0f"
            text: qsTr(angTemp_p.toString())
            font.pointSize: 24
            font.family: "Waseem"
            style: Text.Normal
            font.bold: false
            verticalAlignment: Text.AlignBottom
            horizontalAlignment: Text.AlignHCenter

            Text {
                id: gradoC
                x: 73
                y: 14
                width: 33
                height: 27
                color: "#bd0f0f"
                text: qsTr("ºC")
                font.bold: true
                font.pixelSize: 15
            }
        }

        Text {
            id: text_tiempo_set
            x: 303
            y: 386
            width: 106
            height: 42
            color: "#5a5858"
            text: qsTr((angTiempo_p).toString())
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignBottom
            font.pointSize: 23
            font.family: "Waseem"
            style: Text.Normal
            font.bold: false

            Text {
                id: text_min
                x: 75
                y: 13
                width: 31
                height: 23
                color: "#5a5858"
                text: qsTr("min")
                font.italic: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignBottom
                font.pixelSize: 12
                font.family: "Waseem"
                style: Text.Normal
                font.bold: true
            }
        }

        Text {
            id: temp_actual
            x: 192
            y: 163
            width: 64
            height: 26
            color: "#bd0f0f"
            text: qsTr(temp_mostrar.toString()+" ºC")
            verticalAlignment: Text.AlignBottom
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 35
            font.family: "Droid"
        }

        Text {
            id: tiempo_actual
            x: 192
            y: 204
            width: 64
            height: 44
            color: "#5a5858"
            text:qsTr(stringTiempo)
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 35
            font.family: "Droid"
        }

    }




}
