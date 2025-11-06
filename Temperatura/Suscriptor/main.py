import sys
from PySide6.QtWidgets import QApplication, QMainWindow
from ui_interfaz import Ui_MainWindow
import paho.mqtt.client as mqtt


class MiVentan(QMainWindow, Ui_MainWindow):
    def __init__(self):
        super().__init__()
        self.setupUi(self)
        self.cmdConectar.clicked.connect(self.conectar)
        self.cmdSalir.clicked.connect(self.salir)

        self.client = mqtt.Client()
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message

    def on_connect(self, client, userdata, flags, rc):
        self.client.subscribe("7C/Equipo4/temp1", qos=0)
        self.client.subscribe("7C/Equipo4/temp2", qos=0)
        self.client.subscribe("7C/Equipo4/temp3", qos=0)
        self.client.subscribe("7C/Equipo4/temp4", qos=0)
        print("Suscrito")

    def on_message(self, client, userdata, msg):
        try:
            if msg.topic == "7C/Equipo4/temp1":
                temp1 = str(msg.payload.decode("utf-8"))
                temp1  = int(temp1 )
                self.pbTemp1.setValue(temp1 )
                self.lblTemp1.setText(str(temp1))
            elif msg.topic == "7C/Equipo4/temp2":
                temp2 = str(msg.payload.decode("utf-8"))
                temp2  = int(temp2 )
                self.pbTemp2.setValue(temp2 )
                self.lblTemp2.setText(str(temp2))
            elif msg.topic == "7C/Equipo4/temp3":
                temp3 = str(msg.payload.decode("utf-8"))
                temp3  = int(temp3 )
                self.pbTemp3.setValue(temp3)
                self.lblTemp3.setText(str(temp3))
            elif msg.topic == "7C/Equipo4/temp4":
                temp4 = str(msg.payload.decode("utf-8"))
                temp4  = int(temp4 )
                self.pbTemp4.setValue(temp4)
                self.lblTemp4.setText(str(temp4))
        except:
            print("Fallo al recibir datos")

    def conectar(self):
        self.client.connect(host="test.mosquitto.org", port=1883, keepalive=30)
        #self.client.connect(host="192.168.0.102", port=1883, keepalive=30)
        self.client.loop_start()

    def salir(self):
        try:
            self.client.unsubscribe("7C/Equipo4/temp1")
            self.client.unsubscribe("7C/Equipo4/temp2")
            self.client.unsubscribe("7C/Equipo4/temp3")
            self.client.unsubscribe("7C/Equipo4/temp4")
            self.client.loop_stop()
            self.client.disconnect()
        except:
            print("Fallo al desconectar")
        self.close()


if __name__ == "__main__":
    app = QApplication(sys.argv)
    venta = MiVentan()
    venta.show()
    sys.exit(app.exec())