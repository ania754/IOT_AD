import sqlite3
from PySide6.QtCore import QDateTime
from PySide6.QtGui import QImage, QPixmap
from PySide6.QtWidgets import QGraphicsScene, QDialog, QGraphicsPixmapItem
from matplotlib import pyplot as plt
from matplotlib.backends.backend_agg import FigureCanvasAgg
from graphe import Ui_GraphWindow
from thread_mqtt import id_pc

database_name = "IOT.db"
table_name = 'IOT_Data'

class CourbeDialog(QDialog, Ui_GraphWindow):
    def __init__(self):
        super().__init__()
        self.setupUi(self)
        self.setWindowTitle(f"Courbes historiques - Poste {id_pc}")

        # Initialisation par défaut : dernière heure
        dte = QDateTime.currentDateTime()
        dts = dte.addSecs(-3600)
        self.dteStartTime.setDateTime(dts)
        self.dteEndTime.setDateTime(dte)

        # Connexion des boutons de la boîte de dialogue
        self.btnLoadData.clicked.connect(self.afficher_courbe)
        self.btnClearData.clicked.connect(self.effacer_courbe)

        # Scène graphique pour le QGraphicsView
        self.scene = QGraphicsScene()
        self.graphicsView.setScene(self.scene)

    def figure_to_pixmap(self, fig):
        """Convertit une figure Matplotlib en QPixmap affichable dans Qt"""
        canvas = FigureCanvasAgg(fig)
        canvas.draw()
        width, height = fig.get_size_inches() * fig.get_dpi()
        width, height = int(width), int(height)
        buf = canvas.buffer_rgba()
        image = QImage(buf, width, height, QImage.Format_RGBA8888)
        return QPixmap.fromImage(image)

    def afficher_courbe(self):
        """Récupère les données de SQLite sur l'intervalle et trace les courbes"""
        # On formate explicitement les dates du widget au format texte de la base (DD/MM/YYYY HH:MM:SS)
        debut = self.dteStartTime.dateTime().toString("dd/MM/yyyy HH:mm:ss")
        fin = self.dteEndTime.dateTime().toString("dd/MM/yyyy HH:mm:ss")

        print(f"Recherche entre : {debut} et {fin}") # Petit print pour vérifier dans la console

        conn = sqlite3.connect(database_name)
        cur = conn.cursor()
        cur.execute(
            f"SELECT Date, Temperature, Humidite FROM {table_name} WHERE Date BETWEEN ? AND ? ORDER BY Date ASC",
            (debut, fin)
        )
        rows = cur.fetchall()
        conn.close()

        if not rows:
            print("Aucune donnée trouvée pour cet intervalle de temps.")
            return

        date_x = [r[0] for r in rows]
        temp_y = [r[1] for r in rows]
        humi_y = [r[2] for r in rows]

        self.scene.clear()

        # Création du graphique Matplotlib
        fig, ax = plt.subplots(figsize=(6, 4), dpi=100)
        ax.plot(date_x, temp_y, marker="o", color="red", label="Température (°C)")
        ax.plot(date_x, humi_y, marker="+", color="blue", label="Humidité (%)")
        ax.set_title(f"Données capteur IdO {id_pc}")
        ax.set_xlabel("Date")
        ax.set_ylabel("Valeurs")
        ax.legend(loc="upper left")
        ax.grid(True)
        plt.xticks(rotation=20, fontsize=8)
        fig.tight_layout()

        # Conversion et affichage dans le QGraphicsView
        pixmap = self.figure_to_pixmap(fig)
        item = QGraphicsPixmapItem(pixmap)
        self.scene.addItem(item)
        
        plt.close(fig)

    def effacer_courbe(self):
        """Efface le graphique"""
        self.scene.clear()