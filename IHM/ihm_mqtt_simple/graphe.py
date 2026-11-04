from PySide6.QtCore import QCoreApplication, QMetaObject, Qt
from PySide6.QtWidgets import (
    QDialog, QDateTimeEdit, QGraphicsView, 
    QPushButton, QVBoxLayout, QHBoxLayout
)

class Ui_GraphWindow(object):
    def setupUi(self, GraphWindow):
        GraphWindow.setObjectName("GraphWindow")
        GraphWindow.resize(650, 550)
        GraphWindow.setWindowTitle("Affichage des courbes historiques")
        
        # Layout principal de la boîte de dialogue
        self.verticalLayout = QVBoxLayout(GraphWindow)

        # 1. Ligne de sélection des dates (Début et Fin)
        self.layout_dates = QHBoxLayout()
        
        self.dteStartTime = QDateTimeEdit(GraphWindow)
        self.dteStartTime.setCalendarPopup(True)
        self.layout_dates.addWidget(self.dteStartTime)
        
        self.dteEndTime = QDateTimeEdit(GraphWindow)
        self.dteEndTime.setCalendarPopup(True)
        self.layout_dates.addWidget(self.dteEndTime)
        
        self.verticalLayout.addLayout(self.layout_dates)

        # 2. Zone d'affichage du graphique
        self.graphicsView = QGraphicsView(GraphWindow)
        self.verticalLayout.addWidget(self.graphicsView)

        # 3. Boutons d'action (Charger et Effacer)
        self.layout_buttons = QHBoxLayout()
        
        self.btnLoadData = QPushButton("Charger les données", GraphWindow)
        self.layout_buttons.addWidget(self.btnLoadData)
        
        self.btnClearData = QPushButton("Effacer les courbes", GraphWindow)
        self.layout_buttons.addWidget(self.btnClearData)
        
        self.verticalLayout.addLayout(self.layout_buttons)

        self.retranslateUi(GraphWindow)
        QMetaObject.connectSlotsByName(GraphWindow)

    def retranslateUi(self, GraphWindow):
        pass