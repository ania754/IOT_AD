from client_mqtt import ClientMQTT
from thread_mqtt import ThreadMQTT, topic_version, topic_temperature, topic_humidite, topic_datetime
import time

id_pc = "42904"  # Votre numéro de PC
broker = "192.168.1.1"
port = 1883
username = "mos_user"
password = "mos_user"

def main():
    print("Démarrage de l'application avec Thread")
    print("=====================================")

    # 1. Instanciation du client MQTT
    client_id = f"Client_{id_pc}"
    mqtt = ClientMQTT(broker, port, client_id, username, password)
    mqtt.connect_mqtt()

    # 2. Souscription aux topics
    topics = [topic_version, topic_temperature, topic_humidite, topic_datetime]
    for t in topics:
        print(f"Attente du topic: {t}")
        mqtt.subscribe_mqtt(t)

    # 3. Création et lancement de l'instance de la classe ThreadMQTT
    thread_mqtt = ThreadMQTT(mqtt)
    thread_mqtt.start()
    print("Thread MQTT démarré avec succès.")

    try:
        # Le programme principal continue de tourner (simule une application active)
        while True:
            time.sleep(1)
            
    except KeyboardInterrupt:
        print("\nArrêt de l'application...")
        thread_mqtt.stop_thread()
        thread_mqtt.join()
        mqtt.disconnect_mqtt()
        print("Fin de l'application.")

if __name__ == "__main__":
    main()