import sys
from PySide6.QtCore import Qt
from PySide6.QtWidgets import (
    QApplication, QMainWindow, QWidget, QLabel, QLineEdit,
    QPushButton, QVBoxLayout, QHBoxLayout, QGridLayout, QGroupBox,
    QTableWidget, QTableWidgetItem
)

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(480, 750)  # Fenêtre légèrement agrandie pour l'historique
        MainWindow.setWindowTitle("Mon capteur IDO via MQTT (c)2026 CERI SN - PPI")
        
        # Style sombre pour correspondre au thème de ton TP
        MainWindow.setStyleSheet("""
            QMainWindow { background-color: #2b2b2b; color: #ffffff; }
            QGroupBox { border: 1px solid #555555; border-radius: 4px; margin-top: 10px; font-weight: bold; color: #ffffff; }
            QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 3px; }
            QLabel { color: #ffffff; }
            QLineEdit { background-color: #3b3b3b; color: #ffffff; border: 1px solid #555555; padding: 4px; border-radius: 2px; }
            QPushButton { background-color: #3a3a3a; color: #ffffff; border: 1px solid #666666; padding: 6px; border-radius: 3px; }
            QPushButton:hover { background-color: #4a4a4a; }
            QPushButton:pressed { background-color: #2a2a2a; }
            QTableWidget { background-color: #3b3b3b; color: #ffffff; gridline-color: #555555; border: 1px solid #555555; }
            QHeaderView::section { background-color: #4a4a4a; color: #ffffff; border: 1px solid #555555; padding: 4px; }
        """)

        # Widget central
        self.centralwidget = QWidget(MainWindow)
        main_layout = QVBoxLayout(self.centralwidget)

        # ==================== GROUPE 1 : Paramètres MQTT ====================
        self.groupBox_mqtt = QGroupBox("Paramètres de communication du Broker MQTT", self.centralwidget)
        mqtt_layout = QGridLayout(self.groupBox_mqtt)

        # Logo / Label image placeholder à gauche
        self.lblLogo = QLabel("IMT Nord Europe\nÉcole Mines-Télécom\nIMT-Université de Lille", self.groupBox_mqtt)
        self.lblLogo.setAlignment(Qt.AlignCenter)
        self.lblLogo.setStyleSheet("background-color: #ffffff; color: #004b87; font-weight: bold; padding: 10px; border-radius: 4px;")
        mqtt_layout.addWidget(self.lblLogo, 0, 0, 4, 1)

        # Champs de saisie (Adresse IP, Port, Utilisateur, Mot de passe)
        self.lblIp = QLabel("Adresse IP :", self.groupBox_mqtt)
        self.txtIp = QLineEdit(self.groupBox_mqtt)
        self.txtIp.setText("192.168.1.1")
        mqtt_layout.addWidget(self.lblIp, 0, 1)
        mqtt_layout.addWidget(self.txtIp, 0, 2)

        self.lblPort = QLabel("Port :", self.groupBox_mqtt)
        self.txtPort = QLineEdit(self.groupBox_mqtt)
        self.txtPort.setText("1883")
        mqtt_layout.addWidget(self.lblPort, 1, 1)
        mqtt_layout.addWidget(self.txtPort, 1, 2)

        self.lblUser = QLabel("Utilisateur :", self.groupBox_mqtt)
        self.txtUser = QLineEdit(self.groupBox_mqtt)
        self.txtUser.setText("mos_user")
        mqtt_layout.addWidget(self.lblUser, 2, 1)
        mqtt_layout.addWidget(self.txtUser, 2, 2)

        self.lblPass = QLabel("Mot de passe :", self.groupBox_mqtt)
        self.txtPass = QLineEdit(self.groupBox_mqtt)
        self.txtPass.setEchoMode(QLineEdit.Password)
        self.txtPass.setText("mos_user")
        mqtt_layout.addWidget(self.lblPass, 3, 1)
        mqtt_layout.addWidget(self.txtPass, 3, 2)

        # Boutons Connecter / Déconnecter
        connect_layout = QHBoxLayout()
        self.btnConnect = QPushButton("Connecter", self.groupBox_mqtt)
        self.btnDisconnect = QPushButton("Déconnecter", self.groupBox_mqtt)
        connect_layout.addWidget(self.btnConnect)
        connect_layout.addWidget(self.btnDisconnect)
        mqtt_layout.addLayout(connect_layout, 4, 0, 1, 3)

        main_layout.addWidget(self.groupBox_mqtt)

        # ==================== GROUPE 2 : Affichage des données ====================
        self.groupBox_data = QGroupBox("Affichage des données du capteur", self.centralwidget)
        data_layout = QVBoxLayout(self.groupBox_data)

        self.lblDateTime = QLabel("00/00/0000  00:00:00", self.groupBox_data)
        self.lblDateTime.setAlignment(Qt.AlignCenter)
        self.lblDateTime.setStyleSheet("font-size: 14px; font-weight: bold; color: #a0a0a0;")
        data_layout.addWidget(self.lblDateTime)

        vals_layout = QHBoxLayout()
        self.lblTemp = QLabel("Température (en °C) : --.- °C", self.groupBox_data)
        self.lblHumi = QLabel("Humidité (en %) : --.- %", self.groupBox_data)
        vals_layout.addWidget(self.lblTemp)
        vals_layout.addWidget(self.lblHumi)
        data_layout.addLayout(vals_layout)

        main_layout.addWidget(self.groupBox_data)

        # ==================== GROUPE 3 : Commandes ====================
        self.groupBox_cmd = QGroupBox("Commandes", self.centralwidget)
        cmd_layout = QVBoxLayout(self.groupBox_cmd)

        self.btnSynchroDateTime = QPushButton("Synchronisation de la date et heure du capteur (NTP)", self.groupBox_cmd)
        cmd_layout.addWidget(self.btnSynchroDateTime)

        grid_cmds = QGridLayout()
        self.btnStopMeasure = QPushButton("Arrêter les mesures", self.groupBox_cmd)
        self.btnStartMeasure = QPushButton("Démarrer les mesures", self.groupBox_cmd)
        self.btnSlowBlink = QPushButton("Clignotement lent", self.groupBox_cmd)
        self.btnFastBlink = QPushButton("Clignotement rapide", self.groupBox_cmd)

        grid_cmds.addWidget(self.btnStopMeasure, 0, 0)
        grid_cmds.addWidget(self.btnStartMeasure, 0, 1)
        grid_cmds.addWidget(self.btnSlowBlink, 1, 0)
        grid_cmds.addWidget(self.btnFastBlink, 1, 1)
        cmd_layout.addLayout(grid_cmds)

        # Clignotement personnalisé
        custom_layout = QHBoxLayout()
        self.btnBlink = QPushButton("Clignotement personnalisé", self.groupBox_cmd)
        self.ineBargraph_period = QLineEdit(self.groupBox_cmd)
        self.ineBargraph_period.setText("500")
        lbl_ms = QLabel("ms", self.groupBox_cmd)
        custom_layout.addWidget(self.btnBlink)
        custom_layout.addWidget(self.ineBargraph_period)
        custom_layout.addWidget(lbl_ms)
        cmd_layout.addLayout(custom_layout)

        # Bargraphe
        bargraph_layout = QHBoxLayout()
        lbl_bar = QLabel("Valeur du bargraphe :", self.groupBox_cmd)
        self.ineBargraph = QLineEdit(self.groupBox_cmd)
        bargraph_layout.addWidget(lbl_bar)
        bargraph_layout.addWidget(self.ineBargraph)
        cmd_layout.addLayout(bargraph_layout)

        main_layout.addWidget(self.groupBox_cmd)

        # ==================== GROUPE 4 : Historique SQLite ====================
        self.groupBox_histo = QGroupBox("Historique des mesures (SQLite)", self.centralwidget)
        histo_layout = QVBoxLayout(self.groupBox_histo)

        self.btnLoadHisto = QPushButton("Charger l'historique", self.groupBox_histo)
        histo_layout.addWidget(self.btnLoadHisto)

        # Tableau avec 4 colonnes : Date, Version, Température, Humidité
        self.tableWidget = QTableWidget(self.groupBox_histo)
        self.tableWidget.setColumnCount(4)
        self.tableWidget.setHorizontalHeaderLabels(["Date", "Version", "Température (°C)", "Humidité (%)"])
        histo_layout.addWidget(self.tableWidget)

        main_layout.addWidget(self.groupBox_histo)

        MainWindow.setCentralWidget(self.centralwidget)