from paho.mqtt import client as mqtt_client
import sqlite3

class ClientMQTT:
    def __init__(self, ip_address, port, client_id, username, password):
        self.ip_address = ip_address
        self.port = port
        self.client_id = client_id
        self.username = username
        self.password = password
        self.client = None
        self.conn = None
        self.tablename = "IOT_Data"
        
        # Initialisation des variables pour l'IHM
        self.id_capteur = 0
        self.version = ""
        self.datetime_acq = "00/00/0000 00:00:00"
        self.temperature = 0.0
        self.humidite = 0.0

    def connect_mqtt(self) -> mqtt_client:
        """Établit la connexion avec le broker MQTT"""
        def on_connect(client, userdata, flags, rc, properties=None):
            if rc == 0:
                print(f"Connecté au broker MQTT {self.ip_address} {self.port}")
            else:
                print(f"Échec de la connexion, code d'erreur : {rc}")

        self.client = mqtt_client.Client(mqtt_client.CallbackAPIVersion.VERSION2, client_id=self.client_id)
        
        if self.username and self.password:
            self.client.username_pw_set(self.username, self.password)
            
        self.client.on_connect = on_connect
        self.client.connect(self.ip_address, self.port)
        return self.client

    def disconnect_mqtt(self) -> None:
        """Ferme la connexion avec le broker"""
        if self.client:
            self.client.disconnect()

    def publish_mqtt(self, topic, message, qos=0):
        """Publie un message sur un topic"""
        if self.client:
            self.client.publish(topic, message, qos=qos)

    def subscribe_mqtt(self, topic, qos=0):
        """Souscrit à un topic, gère l'affichage, stocke les valeurs et archive en SQLite"""
        def on_message(client, userdata, msg):
            payload = msg.payload.decode()
            topic_lower = msg.topic.lower()
            print(f"Réception: {payload} depuis '{msg.topic}'")
            
            try:
                self.id_capteur = int(msg.topic.split('_')[-1])
            except ValueError:
                pass

            # Stockage des valeurs selon le topic reçu (en ignorant la casse)
            if "version" in topic_lower:
                self.version = payload
            elif "get_date" in topic_lower:
                self.datetime_acq = payload
            elif "temperature" in topic_lower:
                try:
                    self.temperature = float(payload)
                except ValueError:
                    pass
            elif "humidity" in topic_lower or "humidite" in topic_lower:
                try:
                    self.humidite = float(payload)
                except ValueError:
                    pass

            # Enregistrement sécurisé en base de données si la connexion est active
            if self.conn:
                try:
                    self.conn.cursor().execute(
                        f"INSERT INTO {self.tablename} VALUES (?, ?, ?, ?, ?)",
                        (self.id_capteur, self.datetime_acq, self.version, self.temperature, self.humidite)
                    )
                    self.conn.commit()
                except Exception as e:
                    print(f"Erreur insertion SQLite : {e}")

        if self.client:
            self.client.subscribe(topic, qos=qos)
            # On assigne le callback global (ou cumulatif)
            self.client.on_message = on_message

    def wait_mqtt(self, timeout=0):
        """Attend les données publiées (en boucle ou avec timeout)"""
        if self.client:
            if timeout == 0:
                self.client.loop_forever()
            else:
                self.client.loop(timeout=timeout)

    def connect_sqlite(self, database_name="IOT.db", table_name="IOT_Data"):
        self.tablename = table_name
        self.conn = sqlite3.connect(database_name)
        self.conn.cursor().execute(f"""
            CREATE TABLE IF NOT EXISTS {self.tablename} (
                capteur INTEGER, 
                Date TEXT, 
                Version TEXT, 
                Temperature REAL, 
                Humidite REAL
            )
        """)
        self.conn.commit()

    def disconnect_sqlite(self):
        if self.conn:
            self.conn.close()

    def get_history(self, limit=50):
        """Récupère l'historique des mesures depuis la base SQLite en toute sécurité multi-thread"""
        try:
            # Ouvre une connexion locale dédiée pour la lecture (évite le conflit de threads)
            conn = sqlite3.connect("IOT.db")
            cursor = conn.cursor()
            cursor.execute(f"SELECT Date, Version, Temperature, Humidite FROM {self.tablename} ORDER BY ROWID DESC LIMIT ?", (limit,))
            rows = cursor.fetchall()
            conn.close()
            return rows
        except Exception as e:
            print(f"Erreur lors de la lecture de la bdd : {e}")
            return []