from client_mqtt import ClientMQTT

# Numéro d'identification du pc
id_pc = "42904" 

broker = "192.168.1.1"
port = 1883
username = "mos_user"
password = "mos_user"

# Topiques MQTT selon les consignes du sujet
topic_commande = f"/iot/COMMAND_{id_pc}"
topic_bargraphe = f"/iot/BARGRAPH_{id_pc}"

def main():
    print("Démarrage de l'application")
    print("===========================")
    print(f"Instanciation de votre ClientMQTT (Écriture) pour le capteur : {id_pc}")

    client_id = f"Client_E{id_pc}"
    mqtt = ClientMQTT(broker, port, client_id, username, password)
    
    # Connexion au broker
    mqtt.connect_mqtt()

    car = ""
    while car.upper() != 'Q':
        print("\nConsole capteur IdO")
        print("--------------------")
        print("1 - Mise à l'heure du capteur")
        print("2 - Démarrage des mesures")
        print("3 - Arrêt des mesures")
        print("4 - Clignotement lent")
        print("5 - Clignotement rapide")
        print("6 - Clignotement personnalisé")
        print("7 - Écriture du bargraphe")
        print("Q - Quitter")
        
        car = input("Votre choix : ")
        
        match car.upper():
            case '1':
                # Code commande 10 : Synchronisation date/heure (NTP)
                message = "10"
                print(f"Publication : {message} sur {topic_commande}")
                mqtt.publish_mqtt(topic_commande, message)
                print(f"Synchronisation du capteur : {id_pc}")
                
            case '2':
                # Code commande 20 : Démarrage mesure + Clignotement rapide (50)
                message_mesure = "20"
                print(f"Publication : {message_mesure} sur {topic_commande}")
                mqtt.publish_mqtt(topic_commande, message_mesure)
                print(f"Démarrage des mesures sur le capteur : {id_pc}")
                
                message_blink = "50"
                print(f"Publication : {message_blink} sur {topic_commande}")
                mqtt.publish_mqtt(topic_commande, message_blink)
                
            case '3':
                # Code commande 30 : Arrêt mesure + Clignotement lent (40)
                message_mesure = "30"
                print(f"Publication : {message_mesure} sur {topic_commande}")
                mqtt.publish_mqtt(topic_commande, message_mesure)
                print(f"Arrêt des mesures sur le capteur : {id_pc}")
                
                message_blink = "40"
                print(f"Publication : {message_blink} sur {topic_commande}")
                mqtt.publish_mqtt(topic_commande, message_blink)
                
            case '4':
                # Code commande 40 : Clignotement lent
                message = "40"
                print(f"Publication : {message} sur {topic_commande}")
                mqtt.publish_mqtt(topic_commande, message)
                print(f"Clignotement lent de la led de vie du capteur : {id_pc}")
                
            case '5':
                # Code commande 50 : Clignotement rapide
                message = "50"
                print(f"Publication : {message} sur {topic_commande}")
                mqtt.publish_mqtt(topic_commande, message)
                print(f"Clignotement rapide de la led de vie du capteur : {id_pc}")
                
            case '6':
                # Code commande 60 : Clignotement personnalisé
                message = "60"
                print(f"Publication : {message} sur {topic_commande}")
                mqtt.publish_mqtt(topic_commande, message)
                print(f"Clignotement personnalisé de la led de vie du capteur : {id_pc}")
                
            case '7':
                # Écriture de la valeur sur le bargraphe (0 à 1023)
                valeur = input("Choisir la valeur à écrire sur le bargraphe (0 à 1023) : ")
                print(f"Publication : {valeur} sur {topic_bargraphe}")
                mqtt.publish_mqtt(topic_bargraphe, valeur)
                print(f"Écriture de la valeur {valeur} sur le bargraphe du capteur : {id_pc}")
                
            case 'Q':
                print("Arrêt du client MQTT")
                print("Fin de l'application")
                
            case _:
                print("Choix invalide, veuillez réessayer.")

    # Déconnexion propre
    mqtt.disconnect_mqtt()

if __name__ == "__main__":
    main()