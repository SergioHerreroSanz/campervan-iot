import { CommonModule } from '@angular/common';
import { Component } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { from, Observable, of, tap } from 'rxjs';
import { VentilationFanService } from './ventilation-fan.service';

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
  providers: [VentilationFanService],
})
export class VentilationFanComponent {
  connected$?: Observable<boolean>;
  mode$?: Observable<number>;
  power$?: Observable<number>;
  targetTemp$?: Observable<number>;
  intTemp$?: Observable<number>;
  extTemp$?: Observable<number>;
  rawTemp$?: Observable<number>;

  constructor(private readonly vfs: VentilationFanService) { }

  connectToDevice() {
    this.vfs.connect().then((connected) => {
      this.connected$ = connected;
      this.mode$ = this.vfs.mode$.asObservable();
      this.power$ = this.vfs.power$.asObservable();
      this.targetTemp$ = this.vfs.targetTemp$.asObservable();
      this.intTemp$ = this.vfs.intTemp$.asObservable();
      this.extTemp$ = this.vfs.extTemp$.asObservable();
      this.rawTemp$ = this.vfs.rawTemp$.asObservable().pipe(tap(console.log));
    })

  }

  onSliderChange(event: Event) {
    const target = event.target as HTMLInputElement;
    this.vfs.sendPower(parseInt(target.value, 10));
  }

  onModeChange(event: Event) {
    const target = event.target as HTMLSelectElement;
    this.vfs.sendMode(parseInt(target.value, 10));
  }

  onTargetTempChange(event: Event) {
    const target = event.target as HTMLSelectElement;
    this.vfs.sendTargetTemp(parseFloat(target.value));
  }
}
