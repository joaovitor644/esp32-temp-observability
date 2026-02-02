# ESP32 Observability System

Sistema de observabilidade em tempo real para monitoramento de temperatura utilizando ESP32-C6, com integração via MQTT e stack de visualização local (InfluxDB + Grafana).

---

## 🏗️ Arquitetura do Projeto

O fluxo de dados segue o seguinte percurso:
1.  **Edge (Wokwi):** O ESP32-C6 coleta a temperatura (NTC) e um fator de ganho (Potenciômetro).
2.  **Transporte (AWS):** Os dados são publicados via MQTT no Broker AWS (`18.216.60.2`).
3.  **Ingestão (Python):** O script `bridge.py` consome os dados da nuvem e os grava no banco local.
4.  **Armazenamento (InfluxDB):** Banco de dados de séries temporais para histórico de métricas.
5.  **Visualização (Grafana):** Dashboard para análise gráfica em tempo real.

---

## 🛠️ Configuração do Ambiente

### 1. Simulação (Wokwi)
* **Código:** Utilize o conteúdo de `firmware.cpp`.
* **Sensor de Temperatura (NTC):** Conectado ao pino `0`.
* **Potenciômetro (Ganho):** Conectado ao pino `1`.
* **Display LCD I2C:** Pinos `SDA=6` e `SCL=7`.

### 2. Infraestrutura (Docker)
Na raiz do projeto, execute o comando para subir os serviços:

```bash
docker-compose up -d
```

### 3. Acesso às Interfaces

Após subir o Docker, você pode acessar as ferramentas pelo seu navegador:

    Grafana (Painéis): http://localhost:3000

        Login padrão: 

    InfluxDB (Banco de Dados): http://localhost:8086

        Login: 

### 4. Configurações de Conexão (Grafana)

Para conectar o Grafana ao banco de dados InfluxDB, vá em Connections > Data Sources e use:

    Query Language: Flux

    URL: http://influxdb:8086 (Endereço interno do Docker)

    Organization: 

    Bucket: 

    Token: 

### 5. Servidor de Ingestão (Python)

Dentro da pasta server/, instale as dependências e inicie a ponte de dados:

```bash
pip install paho-mqtt influxdb-client
python bridge.py
```

### 📊 Métricas e Consultas (Flux)

Para visualizar os dados no Grafana, vá para a parte de DashBoard e crie um dashboard. Utilize a seguinte query:
Code snippet

```bash
from(bucket: "sensor_data")
  |> range(start: 0)
  |> filter(fn: (r) => r._measurement == "temperatura_sensor")
  |> filter(fn: (r) => r._field == "real")
  |> yield(name: "Apenas Temperatura")
```


Atributos disponíveis:

    real: Temperatura ambiente medida pelo sensor.

    ganho: Multiplicador ajustado no potenciômetro (1.0 a 10.0).

    total: Valor calculado (Temperatura×Ganho).