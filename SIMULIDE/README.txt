# Partie Simulation - Projet IoT (Simulide & Arduino)

Ce répertoire contient l'ensemble des circuits électroniques simulés sous Simulide ainsi que les programmes associés développés sous Arduino IDE pour la création du capteur intelligent TempHumiX.

---

## Structure des dossiers

Le projet de simulation est découpé en plusieurs sous-dossiers correspondant aux étapes progressives du TP:

* `clignotement_led/` : Premier circuit de prise en main (Arduino Uno et LED clignotante).
* `bargraphe/` : Mise en œuvre d'un bargraphe de LEDs.
* `capteur_dht22/` : Intégration du capteur de température et d'humidité DHT22 avec diodes de seuil.
* `lcd_i2c/` : Affichage sur écran LCD 16x2 via le protocole I2C (AIP31068).
* `lcd_i2c_dht22/` : Combinaison de l'afficheur LCD et du capteur DHT22.
* `TempHumiX/` : Boîtier IoT complet regroupant l'ensemble des composants (Arduino Uno, capteur DHT22, afficheur LCD I2C, bargraphe, LEDs de vie/Rx/Tx/Power et liaison série).

---

## Procédure de compilation et de chargement du code (.hex)

Pour faire fonctionner une simulation sur Simulide à partir de l'Arduino IDE, suivez les étapes suivantes :

1. **Écriture du code :** Ouvrez le fichier source correspondant (ex: `TempHumiX.ino`) dans lbnArduino IDE.
2. **Compilation et exportation du binaire :**
   * Dans l'Arduino IDE, allez dans le menu **Croquis** (Sketch) > **Exporter le binaire compilé (Export Compiled Binary).
   * Cela génère les fichiers `.hex` (compilés pour la cible Arduino) directement dans le dossier de travail du projet.
3. **Chargement dans Simulide :**
   * Ouvrez le fichier de simulation associé sous Simulide (ex: `TempHumiX.sim1`).
   * Faites un **clic droit** sur le microcontrôleur **Arduino Uno** dans l'éditeur de circuit.
   * Sélectionnez l'option pour charger le fichier firmware / code et allez chercher le fichier `.hex` généré par l'Arduino IDE.
4. **Lancement :** Cliquez sur le bouton de démarrage de la simulation en temps réel dans Simulide pour observer le comportement du montage et du moniteur série[cite: 6, 11].

---

## 🚀 Lancement du boîtier global : TempHumiX

Le montage principal du projet se trouve dans le dossier **`TempHumiX/`**. Il simule le boîtier intelligent complet :
* **Fichier de simulation :** `TempHumiX.sim1`
* **Fichier de code :** `TempHumiX.ino`

