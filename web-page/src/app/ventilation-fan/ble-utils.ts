import { BehaviorSubject, from, map, switchMap, tap } from 'rxjs';

export function initCharacteristic<T>(
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

  return from(service.getCharacteristic(uuid)).pipe(
    switchMap((char) =>
      from(char.readValue()).pipe(
        map((value) => {
          const parsed = parser(value);
          subj.next(parsed);
          charCallback(char);
          subCallback(subj);
          return char;
        }),
        tap((char) => listener(char, subj)),
      ),
    ),
  );
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
  listen(char, subj, (v) => v.getUint16(0), 2);
}

export function listenFloat32(
  char: BluetoothRemoteGATTCharacteristic,
  subj: BehaviorSubject<number>,
): void {
  listen(char, subj, (v) => v.getFloat32(0), 4);
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
