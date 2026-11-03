from paho.mqtt import client as mqtt_client

class ClientMQTT:
    def __init__(self, ip_address, port, client_id, username, password):
        self.ip_address = ip_address
        self.port = port
        self.client_id = client_id
        self.username = username
        self.password = password
        self.client = None

    def connect_mqtt(self) -> mqtt_client:
        """Établit la connexion avec le broker MQTT"""
        def on_connect(client, userdata, flags, rc, properties=None):
            if rc == 0:
                print(f"Connecté au broker MQTT {self.ip_address} {self.port}")
            else:
                print(f"Échec de la connexion, code d'erreur : {rc}")

        # Initialisation du client MQTT (version paho-mqtt récente)
        self.client = mqtt_client.Client(mqtt_client.CallbackAPIVersion.VERSION2, client_id=self.client_id)
        
        # Authentification
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
        """Souscrit à un topic"""
        def on_message(client, userdata, msg):
            print(f"Réception: {msg.payload.decode()} depuis '{msg.topic}'")

        if self.client:
            self.client.subscribe(topic, qos=qos)
            self.client.on_message = on_message

    def wait_mqtt(self, timeout=0):
        """Attend les données publiées (en boucle ou avec timeout)"""
        if self.client:
            if timeout == 0:
                self.client.loop_forever()
            else:
                self.client.loop(timeout=timeout)

    