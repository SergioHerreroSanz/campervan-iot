import { Component } from '@angular/core';

@Component({
  selector: 'app-ventilation-fan',
  templateUrl: './ventilation-fan.component.html',
  styleUrl: './ventilation-fan.component.scss',
})
export class VentilationFanComponent {
  sliderValue = 0;
  device?: BluetoothDevice;
  characteristic?: BluetoothRemoteGATTCharacteristic;

  // Usa los UUID reales que estés utilizando
  readonly SERVICE_UUID = '4fafc201-1fb5-459e-8fcc-c5c9c331914b';
  readonly CHARACTERISTIC_UUID = 'beb5483e-36e1-4688-b7f5-ea07361b26a8';

  // Función para conectar al dispositivo BLE
  async connectToDevice() {
    try {
      this.device = await navigator.bluetooth.requestDevice({
        filters: [{ namePrefix: 'ESP32' }],
        optionalServices: [this.SERVICE_UUID],
      });

      const server = await this.device.gatt?.connect();
      const service = await server?.getPrimaryService(this.SERVICE_UUID);
      this.characteristic = await service?.getCharacteristic(
        this.CHARACTERISTIC_UUID
      );

      // Leer el valor actual desde la característica
      const value = await this.characteristic?.readValue();
      if (value) {
        this.sliderValue = value.getUint16(0, true);
        console.log('Valor actual recibido:', this.sliderValue);
      }

      console.log('✅ Conectado y característica lista');
    } catch (error) {
      console.error('❌ Error al conectar:', error);
    }
  }

  // Función para manejar el cambio del slider
  async onSliderChange(event: Event) {
    const input = event.target as HTMLInputElement;
    this.sliderValue = parseInt(input.value, 10);

    if (this.characteristic) {
      try {
        const data = new Uint16Array([this.sliderValue]);
        await this.characteristic.writeValue(data);
        console.log(`📤 Enviado: ${this.sliderValue}`);
      } catch (error) {
        console.error('❌ Error al enviar valor:', error);
      }
    } else {
      console.warn('⚠️ Característica BLE no disponible');
    }
  }
}
