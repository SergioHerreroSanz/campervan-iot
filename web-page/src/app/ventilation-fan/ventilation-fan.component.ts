import { CommonModule } from '@angular/common';
import { Component } from '@angular/core';
import { FormsModule } from '@angular/forms';

export enum FanMode {
  FAN_MODE_OFF = 0,
  FAN_MODE_MANUAL = 1,
  FAN_MODE_AUTO = 2,
}

@Component({
  selector: 'app-ventilation-fan',
  templateUrl: './ventilation-fan.component.html',
  styleUrl: './ventilation-fan.component.scss',
  imports: [CommonModule, FormsModule],
})
export class VentilationFanComponent {
  device?: BluetoothDevice;
  powerCharacteristic?: BluetoothRemoteGATTCharacteristic;
  modeCharacteristic?: BluetoothRemoteGATTCharacteristic;
  targetTempCharacteristic?: BluetoothRemoteGATTCharacteristic;

  fanMode: FanMode = FanMode.FAN_MODE_MANUAL;
  powerValue = 0;
  modeValue = 0;
  targetTempValue = 0;

  // Usa los UUID reales que estés utilizando
  readonly SERVICE_UUID = '4fafc201-1fb5-459e-8fcc-c5c9c331914b';
  readonly POWER_CHARACTERISTIC_UUID = 'beb5483e-36e1-4688-b7f5-ea07361b26a8';
  readonly MODE_CHARACTERISTIC_UUID = '6aee789a-4bcd-4c90-a8ef-456dfe253ad1';
  readonly TARGET_TEMP_CHARACTERISTIC_UUID =
    '6aee789a-4bcd-4c90-a8ef-456dfe253ad2';

  // Función para conectar al dispositivo BLE
  async connectToDevice() {
    try {
      this.device = await navigator.bluetooth.requestDevice({
        filters: [{ namePrefix: 'ESP32' }],
        optionalServices: [this.SERVICE_UUID],
      });

      const server = await this.device.gatt?.connect();
      const service = await server?.getPrimaryService(this.SERVICE_UUID);
      this.powerCharacteristic = await service?.getCharacteristic(
        this.POWER_CHARACTERISTIC_UUID
      );
      this.modeCharacteristic = await service?.getCharacteristic(
        this.MODE_CHARACTERISTIC_UUID
      );
      this.targetTempCharacteristic = await service?.getCharacteristic(
        this.TARGET_TEMP_CHARACTERISTIC_UUID
      );

      // Leer el valor actual desde la característica
      const powerValue = await this.powerCharacteristic?.readValue();
      if (powerValue) {
        this.powerValue = powerValue.getUint16(0, true);
        console.log('Valor PWM recibido:', this.powerValue);
      }
      const modeValue = await this.modeCharacteristic?.readValue();
      if (modeValue) {
        this.modeValue = modeValue.getUint8(0);
        console.log('Modo recibido:', this.modeValue);
      }
      const targetTempValue = await this.targetTempCharacteristic?.readValue();
      if (targetTempValue && targetTempValue.byteLength >= 4) {
        this.targetTempValue = targetTempValue.getFloat32(0, true);
        console.log('Temperatura objetivo recibido:', this.targetTempValue);
      }

      console.log('✅ Conectado y característica lista');
    } catch (error) {
      console.error('❌ Error al conectar:', error);
    }
  }

  // Función para manejar el cambio del slider
  async onSliderChange(event: Event) {
    const input = event.target as HTMLInputElement;
    this.powerValue = parseInt(input.value, 10);

    if (this.powerCharacteristic) {
      try {
        const data = new Uint16Array([this.powerValue]);
        await this.powerCharacteristic.writeValue(data);
        console.log(`📤 Enviado: ${this.powerValue}`);
      } catch (error) {
        console.error('❌ Error al enviar valor:', error);
      }
    } else {
      console.warn('⚠️ Característica BLE no disponible');
    }
  }

  async onModeChange(event: Event) {
    const select = event.target as HTMLSelectElement;
    const selectedMode = parseInt(select.value, 10);

    // Actualiza el modo y envíalo al dispositivo
    this.fanMode =
      selectedMode === 0
        ? FanMode.FAN_MODE_OFF
        : selectedMode === 1
        ? FanMode.FAN_MODE_MANUAL
        : FanMode.FAN_MODE_AUTO;

    if (this.powerCharacteristic) {
      try {
        const modeData = new Uint8Array([this.fanMode]);
        this.modeCharacteristic?.writeValue(modeData);
        console.log(
          `📤 Modo enviado: ${
            this.fanMode === FanMode.FAN_MODE_MANUAL ? 'Manual' : 'Automático'
          }`
        );
      } catch (error) {
        console.error('❌ Error al enviar el modo:', error);
      }
    } else {
      console.warn('⚠️ Característica BLE no disponible para el modo');
    }
  }

  async onTargetTempChange(event: Event) {
    const select = event.target as HTMLSelectElement;
    const targetTemp = parseFloat(select.value);

    this.targetTempValue = targetTemp;

    if (this.targetTempCharacteristic) {
      try {
        const buffer = new ArrayBuffer(4); // espacio para un float de 32 bits
        new DataView(buffer).setFloat32(0, targetTemp, true); // little endian

        await this.targetTempCharacteristic?.writeValue(buffer);
        console.log('Temperatura objetivo enviada:', targetTemp);
      } catch (error) {
        console.error('❌ Error al enviar el modo:', error);
      }
    } else {
      console.warn('⚠️ Característica BLE no disponible para el modo');
    }
  }
}
