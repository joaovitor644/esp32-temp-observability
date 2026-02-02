import paho.mqtt.client as mqtt
from influxdb_client import InfluxDBClient, Point, WritePrecision
from influxdb_client.client.write_api import SYNCHRONOUS
import json

# --- Configurações que vieram do João Vitor ---
MQTT_BROKER = "18.216.60.2"
MQTT_TOPIC = "esp32c6/observability/sensor1"
MQTT_PORT = 1883

# --- Configurações do seu Docker Local ---
INFLUX_URL = "http://localhost:8086"
INFLUX_TOKEN = "meu-token-secreto-123"
INFLUX_ORG = "meu_projeto"
INFLUX_BUCKET = "sensor_data"

# Inicializando a conexão com o InfluxDB
client_influx = InfluxDBClient(url=INFLUX_URL, token=INFLUX_TOKEN, org=INFLUX_ORG)
write_api = client_influx.write_api(write_options=SYNCHRONOUS)

def on_message(client, userdata, msg):
    try:
        # 1. O que o 'Listen' fazia: Receber e mostrar o dado bruto
        corpo_mensagem = msg.payload.decode()
        print(f"\n[AWS -> MQTT] Chegou: {corpo_mensagem}")
        
        # 2. Transformar o texto em um objeto (JSON)
        dados = json.loads(corpo_mensagem)
        
        # 3. O que o 'Bridge' faz: Preparar para o Banco de Dados
        ponto = Point("temperatura_sensor") \
            .tag("dispositivo", "esp32c6") \
            .field("real", dados["real_temp"]) \
            .field("ganho", dados["ganho"]) \
            .field("total", dados["total"])
        
        # 4. Enviar para o InfluxDB
        write_api.write(bucket=INFLUX_BUCKET, record=ponto)
        print(f"[PC -> InfluxDB] Sucesso! Dados de {dados['real_temp']}°C salvos.")

    except Exception as e:
        print(f"Erro ao processar mensagem: {e}")

# Configuração do "Carteiro" MQTT
mqtt_client = mqtt.Client()
mqtt_client.on_message = on_message

print(f"Conectando ao Broker AWS ({MQTT_BROKER})...")
mqtt_client.connect(MQTT_BROKER, MQTT_PORT)
mqtt_client.subscribe(MQTT_TOPIC)

print("Ponte Ativa! Agora você verá os dados aqui e no Grafana.")
mqtt_client.loop_forever()