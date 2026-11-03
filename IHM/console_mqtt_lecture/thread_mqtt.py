from threading import Thread

id_pc = "42904"  

topic_version = f"/iot/VERSION_{id_pc}"
topic_temperature = f"/iot/TEMPERATURE_{id_pc}"
topic_humidite = f"/iot/HUMIDITY_{id_pc}"
topic_datetime = f"/iot/GET_DATE_{id_pc}"

class ThreadMQTT(Thread):
    def __init__(self, client_mqtt):
        super().__init__()
        self.mqtt = client_mqtt
        self.stop = False

    def run(self):
        while not self.stop:
            self.mqtt.wait_mqtt(timeout=1.0)

    def stop_thread(self):
        self.stop = True