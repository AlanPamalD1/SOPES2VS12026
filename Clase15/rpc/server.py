import grpc
from concurrent import futures
import time

# Importamos las clases generadas por protoc
import sensor_pb2
import sensor_pb2_grpc

class TemperatureMonitorServicer(sensor_pb2_grpc.TemperatureMonitorServicer):
    
    # Implementamos el método definido en el archivo .proto
    def ReportTemperature(self, request, context):
        print(f"[SERVIDOR] Recibida lectura del dispositivo '{request.device_id}': {request.celsius}°C")
        
        # Lógica de evaluación (como en el ejemplo de FreeRTOS)
        if request.celsius > 28:
            msg = "¡ALERTA! Temperatura crítica registrada en el servidor."
            print(f"[SERVIDOR] {msg}")
            return sensor_pb2.AckResponse(success=True, message=msg)
        else:
            return sensor_pb2.AckResponse(success=True, message="Lectura normal registrada.")

def serve():
    # Creamos un servidor gRPC con un pool de hilos para manejar múltiples sensores
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    
    # Vinculamos nuestra clase lógica al servidor
    sensor_pb2_grpc.add_TemperatureMonitorServicer_to_server(TemperatureMonitorServicer(), server)
    
    # Escuchamos en el puerto 50051
    server.add_insecure_port('[::]:50051')
    server.start()
    print("Servidor gRPC de Monitoreo Térmico iniciado en el puerto 50051...")
    server.wait_for_termination()

if __name__ == '__main__':
    serve()