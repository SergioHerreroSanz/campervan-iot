import { Injectable } from '@angular/core';
import {
  BehaviorSubject,
  catchError,
  combineLatest,
  forkJoin,
  from,
  map,
  Observable,
  of,
  switchMap,
  tap,
} from 'rxjs';
import {
  initCharacteristic,
  listenFloat32,
  listenUint16,
  listenUint8,
} from './ble-utils';

export const BLE_NAME_PREFIX = 'ESP32';
export const BLE_SERVICE_UID = '4fafc201-1fb5-459e-8fcc-c5c9c331914b';
export const BLE_POWER_UID = 'beb5483e-36e1-4688-b7f5-ea07361b26a8';
export const BLE_MODE_UID = '6aee789a-4bcd-4c90-a8ef-456dfe253ad1';
export const BLE_TARGET_TEMP_UID = '6aee789a-4bcd-4c90-a8ef-456dfe253ad2';
export const BLE_CURRENT_POWER_UID = '6aee789a-4bcd-4c90-a8ef-456dfe253ad3';
export const BLE_EXT_TEMP_UID = '6aee789a-4bcd-4c90-a8ef-456dfe253ad4';
export const BLE_RAW_TEMP_UID = '6aee789a-4bcd-4c90-a8ef-456dfe253ad5';

@Injectable()
export class VentilationFanService {
  device?: BluetoothDevice;
  modeC?: BluetoothRemoteGATTCharacteristic;
  userPowerC?: BluetoothRemoteGATTCharacteristic;
  targetTempC?: BluetoothRemoteGATTCharacteristic;
  currPowerC?: BluetoothRemoteGATTCharacteristic;
  rawTempC?: BluetoothRemoteGATTCharacteristic;

  mode$: BehaviorSubject<number> = new BehaviorSubject(0);
  userPower$: BehaviorSubject<number> = new BehaviorSubject(0);
  targetTemp$: BehaviorSubject<number> = new BehaviorSubject(0);
  currPower$: BehaviorSubject<number> = new BehaviorSubject(0);
  rawTemp$: BehaviorSubject<number> = new BehaviorSubject(0);

  async connect(): Promise<Observable<boolean>> {
    try {
      const device = await navigator.bluetooth.requestDevice({
        filters: [{ namePrefix: BLE_NAME_PREFIX }],
        optionalServices: [BLE_SERVICE_UID],
      })
      this.device = device;
      const server = await this.device.gatt?.connect();
      if (server) {
        const s = await server.getPrimaryService(BLE_SERVICE_UID)
        await initCharacteristic(s, BLE_MODE_UID, listenUint8, (dv) => dv.getUint8(0), (c) => (this.modeC = c), (bs) => (this.mode$ = bs));
        await initCharacteristic(s, BLE_POWER_UID, listenFloat32, (dv) => dv.getFloat32(0, true), (c) => (this.userPowerC = c), (bs) => (this.userPower$ = bs));
        await initCharacteristic(s, BLE_TARGET_TEMP_UID, listenFloat32, (dv) => dv.getFloat32(0, true), (c) => (this.targetTempC = c), (bs) => (this.targetTemp$ = bs));
        await initCharacteristic(s, BLE_CURRENT_POWER_UID, listenFloat32, (dv) => dv.getFloat32(0, true), (c) => (this.currPowerC = c), (bs) => (this.currPower$ = bs));
        await initCharacteristic(s, BLE_RAW_TEMP_UID, listenFloat32, (dv) => dv.getFloat32(0, true), (c) => (this.rawTempC = c), (bs) => (this.rawTemp$ = bs));

        console.log('✅ Conectado y características listas');
        console.log('Modo recibido:', this.mode$.value);
        console.log('Potencia manual recibida:', this.userPower$.value);
        console.log('Temp objetivo recibida:', this.targetTemp$.value);
        console.log('Potencia recibida:', this.currPower$.value);
        console.log('Temp recibida:', this.rawTemp$.value);

        const connected$ = new BehaviorSubject(!!this.device?.gatt?.connected)
        device?.addEventListener('gattserverdisconnected', (e) => {
          connected$.next(!!(e.target as BluetoothDevice).gatt?.connected)
        })
        return connected$.asObservable()
      }
    } catch (error) {
      console.error('❌ Error en la conexión BLE:', error);
    }

    return of(false);
  }

  sendMode(mode: number): void {
    if (this.modeC) {
      try {
        this.modeC?.writeValue(new Uint8Array([mode]));
        console.log(`📤 Modo enviado: ${mode}`);
      } catch (error) {
        console.error('❌ Error al enviar el modo:', error);
      }
    } else {
      console.warn('⚠️ Característica BLE no disponible');
    }
  }

  sendPower(power: number): void {
    if (this.userPowerC) {
      try {
        const buffer = new ArrayBuffer(4);
        new DataView(buffer).setFloat32(0, power, true);
        this.userPowerC?.writeValue(buffer);
        console.log(`📤 Potencia enviada: ${power}`);
      } catch (error) {
        console.error('❌ Error al enviar potencia:', error);
      }
    } else {
      console.warn('⚠️ Característica BLE no disponible');
    }
  }

  sendTargetTemp(targetTemp: number): void {
    if (this.targetTempC) {
      try {
        const buffer = new ArrayBuffer(4);
        new DataView(buffer).setFloat32(0, targetTemp, true);

        this.targetTempC?.writeValue(buffer);
        console.log('Temperatura objetivo enviada:', targetTemp);
      } catch (error) {
        console.error('❌ Error al enviar la temperatura objetivo:', error);
      }
    } else {
      console.warn('⚠️ Característica BLE no disponible');
    }
  }
}
