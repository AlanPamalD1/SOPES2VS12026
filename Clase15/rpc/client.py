import grpc
import random
import time

import sensor_pb2
import sensor_pb2_grpc

def run():
    # 1. Abrimos un canal de comunicación hacia el servidor
    with grpc.insecure_channel('localhost:50051') as channel:
        
        # 2. Creamos el 'Stub' (el cliente que abstrae la llamada de red)
        stub = sensor_pb2_grpc.TemperatureMonitorStub(channel)
        
        print("Iniciando envío de telemetría...")
        
        # Simulamos el envío de 5 lecturas consecutivas
        for _ in range(5):
            temp_simulada = random.randint(20, 35)
            
            # 3. Empaquetamos los datos en la estructura Protobuf
            data = sensor_pb2.TemperatureData(device_id="ESP32_Planta_1", celsius=temp_simulada)
            
            print(f"[CLIENTE] Enviando: {temp_simulada}°C...")
            
            # 4. Invocamos el Procedimiento Remoto (RPC)
            response = stub.ReportTemperature(data)
            
            print(f"[CLIENTE] Respuesta del servidor: {response.message}\n")
            time.sleep(2)

if __name__ == '__main__':
    run()