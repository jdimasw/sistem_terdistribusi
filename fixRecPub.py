import RPi.GPIO as GPIO
from lib_nrf24 import NRF24
import time
import datetime
import spidev
#import paho.mqtt.publish as publish
import json

#Import library paho mqtt
import paho.mqtt.client as mqtt

GPIO.setmode(GPIO.BCM)

pipes = [[0xE8, 0xE8, 0xF0, 0xF0, 0xE1], [0xF0, 0xF0, 0xF0, 0xF0, 0xE1]]

radio = NRF24(GPIO, spidev.SpiDev())
radio.begin(0, 17)

radio.setPayloadSize(32)
radio.setChannel(0x76)
radio.setDataRate(NRF24.BR_1MBPS)
radio.setPALevel(NRF24.PA_MIN)

radio.setAutoAck(True)
radio.enableDynamicPayloads()
radio.enableAckPayload()

radio.openReadingPipe(1, pipes[1])
radio.openReadingPipe(2, pipes[0])
radio.printDetails()
radio.startListening()

#Inisiasi mqtt client dengan Id Random
mqtt_client = mqtt.Client()

#Koneksi ke broker
mqtt_client.connect("broker.hivemq.com", 1883)

dataInit = {}


while(1):
    # ackPL = [1]
    while not radio.available(0):
        time.sleep(1 / 100)
    receivedMessage = []
    radio.read(receivedMessage, radio.getDynamicPayloadSize())
    #print("Received: {}".format(receivedMessage))

    #print("Translating the receivedMessage into unicode characters")
    string = ""
    for n in receivedMessage:
        # Decode into standard unicode set
        if (n >= 32 and n <= 126):
            string += chr(n)
    #print("Received message: {}".format(string))
    if string:
        ts = time.time()
        timestamp = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')
        data = json.loads(string)
        data["timestamp"] = timestamp
        
        #dataKirim = json.loads(dataInit)        
        dataInit["id_device"] = data["id"]
        dataInit["t_total"] = data["height"]
        dataInit["t_sampah"] = data["val"]
        dataInit["persentase"] = 0
        dataInit["timestamp"] = timestamp
        
        #if data["id"] == 1:
         #   data["loc"] = "Gedung C"
        #elif data["id"] == 2:
         #   data["loc"] = "Gedung A"
        
        publishData = json.dumps(dataInit)
                
        #publish.single("/skt/test", publishData, hostname="test.mosquitto.org")
        mqtt_client.publish("/skt/test", publishData)
        print(json.dumps(dataInit))
        print("Publish done")

#Publish sebuah message
#mqtt_client.publish("/sensor/suhu/1", "20")
#mqtt_client.publish("/sensor/suhu/2", "30")
#mqtt_client.publish("/sensor/kelembaban/1", "80%")
#mqtt_client.publish("/sensor/kelembaban/2", "90%")
