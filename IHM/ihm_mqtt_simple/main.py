import sys
from PySide6.QtCore import QTimer
from PySide6.QtWidgets import QApplication, QMainWindow, QTableWidgetItem
from mon_ihm import Ui_MainWindow
from client_mqtt import ClientMQTT
from thread_mqtt import ThreadMQTT, id_pc

class MonIHM(QMainWindow, Ui_MainWindow):
    def __init__(self):
        super().__init__()
        self.setupUi(self)
        self.setWindowTitle(f"IHM MQTT - Poste {id_pc}")

        self.client_mqtt = None
        self.thread_mqtt = None

        # Timer pour rafraîchir l'affichage de l'IHM toutes les secondes (1000 ms)
        self.timer = QTimer()
        self.timer.timeout.connect(self.display)

        # --- LIAISON DES BOUTONS DE L'INTERFACE ---
        if hasattr(self, 'btnConnect'):
            self.btnConnect.clicked.connect(self.lancer_connexion)
        if hasattr(self, 'btnDisconnect'):
            self.btnDisconnect.clicked.connect(self.disconnectMQTT)

        if hasattr(self, 'btnSynchroDateTime'):
            self.btnSynchroDateTime.clicked.connect(self.synchroDateTime)
        if hasattr(self, 'btnStartMeasure'):
            self.btnStartMeasure.clicked.connect(self.startMeasure)
        if hasattr(self, 'btnStopMeasure'):
            self.btnStopMeasure.clicked.connect(self.stopMeasure)
        if hasattr(self, 'btnSlowBlink'):
            self.btnSlowBlink.clicked.connect(self.slowBlink)
        if hasattr(self, 'btnFastBlink'):
            self.btnFastBlink.clicked.connect(self.fastBlink)
        if hasattr(self, 'btnBlink'):
            self.btnBlink.clicked.connect(self.customBlink)
        
        # Bargraphe (validation avec la touche Entrée)
        if hasattr(self, 'ineBargraph'):
            self.ineBargraph.returnPressed.connect(self.setBargraph)

        # --- AJOUT : Bouton pour charger l'historique SQLite ---
        if hasattr(self, 'btnLoadHisto'):
            self.btnLoadHisto.clicked.connect(self.chargerHistorique)

    def lancer_connexion(self):
        broker = self.txtIp.text() if hasattr(self, 'txtIp') else "192.168.1.1"
        port = int(self.txtPort.text()) if hasattr(self, 'txtPort') else 1883
        username = self.txtUser.text() if hasattr(self, 'txtUser') else "mos_user"
        password = self.txtPass.text() if hasattr(self, 'txtPass') else "mos_user"

        print("Lancement de la connexion MQTT...")
        self.client_mqtt = ClientMQTT(broker, port, f"IHM_{id_pc}", username, password)
        self.client_mqtt.connect_mqtt()

        # Souscription aux topics du capteur (en MAJUSCULES)
        self.client_mqtt.subscribe_mqtt(f"/iot/VERSION_{id_pc}")
        self.client_mqtt.subscribe_mqtt(f"/iot/TEMPERATURE_{id_pc}")
        self.client_mqtt.subscribe_mqtt(f"/iot/HUMIDITY_{id_pc}")
        self.client_mqtt.subscribe_mqtt(f"/iot/GET_DATE_{id_pc}")

        # Démarrage du thread d'écoute en arrière-plan
        self.thread_mqtt = ThreadMQTT(self.client_mqtt)
        self.thread_mqtt.start()

        # Démarrage du timer d'affichage
        self.timer.start(1000)
        print("Connecté, abonnements faits et thread démarré avec succès !")

    def disconnectMQTT(self):
        self.timer.stop()
        if self.thread_mqtt:
            self.thread_mqtt.stop_thread()
        if self.client_mqtt:
            self.client_mqtt.disconnect_mqtt()
        print("Déconnecté du broker MQTT.")

    def display(self):
        if self.client_mqtt:
            if hasattr(self.client_mqtt, 'datetime_acq') and hasattr(self, 'lblDateTime'):
                self.lblDateTime.setText(str(self.client_mqtt.datetime_acq))
            if hasattr(self.client_mqtt, 'temperature') and hasattr(self, 'lblTemp'):
                self.lblTemp.setText(f"Température (en °C) : {self.client_mqtt.temperature} °C")
            if hasattr(self.client_mqtt, 'humidite') and hasattr(self, 'lblHumi'):
                self.lblHumi.setText(f"Humidité (en %) : {self.client_mqtt.humidite} %")

    

    # --- COMMANDES DU CAPTEUR (Topics en MAJUSCULES) ---
    def synchroDateTime(self):
        topic = f"/iot/COMMAND_{id_pc}"
        if self.client_mqtt:
            self.client_mqtt.publish_mqtt(topic, "10")
        print(f"Publication : 10 sur {topic}")

    def startMeasure(self):
        topic = f"/iot/COMMAND_{id_pc}"
        if self.client_mqtt:
            self.client_mqtt.publish_mqtt(topic, "20")
            self.client_mqtt.publish_mqtt(topic, "50")
        print(f"Démarrage mesures sur {id_pc}")

    def stopMeasure(self):
        topic = f"/iot/COMMAND_{id_pc}"
        if self.client_mqtt:
            self.client_mqtt.publish_mqtt(topic, "30")
            self.client_mqtt.publish_mqtt(topic, "40")
        print(f"Arrêt mesures sur {id_pc}")

    def slowBlink(self):
        topic = f"/iot/COMMAND_{id_pc}"
        if self.client_mqtt:
            self.client_mqtt.publish_mqtt(topic, "40")
        print("Clignotement lent")

    def fastBlink(self):
        topic = f"/iot/COMMAND_{id_pc}"
        if self.client_mqtt:
            self.client_mqtt.publish_mqtt(topic, "50")
        print("Clignotement rapide")

    def customBlink(self):
        topic_cmd = f"/iot/COMMAND_{id_pc}"
        topic_periode = f"/iot/BLINK_{id_pc}"
        periode = self.ineBargraph_period.text() if hasattr(self, 'ineBargraph_period') else "500"
        if self.client_mqtt:
            self.client_mqtt.publish_mqtt(topic_cmd, "60")
            self.client_mqtt.publish_mqtt(topic_periode, periode)
        print(f"Clignotement personnalisé : {periode}ms")

    def setBargraph(self):
        topic = f"/iot/BARGRAPH_{id_pc}"
        valeur = self.ineBargraph.text() if hasattr(self, 'ineBargraph') else "0"
        if self.client_mqtt:
            self.client_mqtt.publish_mqtt(topic, valeur)
        print(f"Bargraphe envoyé : {valeur} sur {topic}")

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MonIHM()
    window.show()
    sys.exit(app.exec())