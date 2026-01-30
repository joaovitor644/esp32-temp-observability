# ESP32 Observability System

Sistema de observabilidade para leituras de temperatura com ESP32-C6,
publicação via MQTT e visualização em tempo real com InfluxDB + Grafana.

## Estrutura

- `firmware/` → código do microcontrolador
- `server/` → serviço Python para ingestão de métricas
- `influxdb/` → configuração de banco de dados e dashboards
- `docs/` → documentação do projeto

## Tecnologias

- ESP32-C6 (simulado)
- MQTT (Mosquitto)
- Python (paho-mqtt, influxdb_client)
- InfluxDB
- Grafana

## Como usar

1. Configurar Wi-Fi no `firmware/`
2. Subir broker MQTT
3. Executar servidor Python
4. Configurar InfluxDB
5. Importar dashboard no Grafana
