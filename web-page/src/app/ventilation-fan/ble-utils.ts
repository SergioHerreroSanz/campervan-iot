import { BehaviorSubject, delay, from, map, of, switchMap, tap } from 'rxjs';

export async function initCharacteristic<T>(
  service: BluetoothRemoteGATTService,
  uuid: string,
  listener: (
    char: BluetoothRemoteGATTCharacteristic,
    subj: BehaviorSubject<T>,
  ) => void,
  parser: (dv: DataView) => T,
  charCallback: (c: BluetoothRemoteGATTCharacteristic) => void,
  subCallback: (bs: BehaviorSubject<T>) => void,
) {
  const subj = new BehaviorSubject<T>(null as any);
  const char = await service.getCharacteristic(uuid);
  const value = await char.readValue()
  const parsed = parser(value);
  subj.next(parsed);
  charCallback(char);
  subCallback(subj);
  listener(char, subj)
  await new Promise(r => setTimeout(r, 100));
}

export function listenUint8(
  char: BluetoothRemoteGATTCharacteristic,
  subj: BehaviorSubject<number>,
): void {
  listen(char, subj, (v) => v.getUint8(0), 1);
}

export function listenUint16(
  char: BluetoothRemoteGATTCharacteristic,
  subj: BehaviorSubject<number>,
): void {
  listen(char, subj, (v) => v.getUint16(0, true), 2);
}

export function listenFloat32(
  char: BluetoothRemoteGATTCharacteristic,
  subj: BehaviorSubject<number>,
): void {
  listen(char, subj, (v) => v.getFloat32(0, true), 4);
}

export function listen(
  char: BluetoothRemoteGATTCharacteristic,
  subj: BehaviorSubject<number>,
  parseCallback: (v: DataView) => number,
  minBytes: number,
) {
  char?.startNotifications().then(() => {
    char?.addEventListener('characteristicvaluechanged', (event: Event) => {
      const v = (event.target as BluetoothRemoteGATTCharacteristic).value;
      if (v && v.byteLength >= minBytes) {
        subj.next(parseCallback(v));
      }
    });
  });
}
