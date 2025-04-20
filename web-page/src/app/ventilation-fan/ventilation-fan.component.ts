import { CommonModule } from '@angular/common';
import { Component } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { from, map, Observable, of, pipe, take, tap } from 'rxjs';
import { VentilationFanService } from './ventilation-fan.service';

export enum FanMode {
  FAN_MODE_OFF = 0,
  FAN_MODE_MANUAL = 1,
  FAN_MODE_TURBO = 2,
  FAN_MODE_AUTO_LOW = 3,
  FAN_MODE_AUTO_HIGH = 4,
  FAN_MODE_SILENT = 5
}

@Component({
  selector: 'app-ventilation-fan',
  templateUrl: './ventilation-fan.component.html',
  styleUrl: './ventilation-fan.component.scss',
  imports: [CommonModule, FormsModule],
  providers: [VentilationFanService],
})
export class VentilationFanComponent {
  availableModes = FanMode;
  resolution = 100;
  autoMode = false;

  connected$?: Observable<boolean>;
  mode$?: Observable<number>;
  userPower$?: Observable<number>;
  targetTemp$?: Observable<number>;
  currPower$?: Observable<number>;
  rawTemp$?: Observable<number>;

  constructor(private readonly vfs: VentilationFanService) { }

  connectToDevice() {
    this.vfs.connect().then((connected) => {
      this.connected$ = connected;
      this.mode$ = this.vfs.mode$.asObservable().pipe(tap(val => {
        if (val == FanMode.FAN_MODE_AUTO_LOW || val == FanMode.FAN_MODE_AUTO_HIGH) {
          this.autoMode = val == FanMode.FAN_MODE_AUTO_HIGH
        }
      }));
      this.userPower$ = this.vfs.userPower$.asObservable().pipe(map(val => Math.round(val * this.resolution)));
      this.targetTemp$ = this.vfs.targetTemp$.asObservable().pipe(map(val => Math.round(val * 10) / 10));
      this.currPower$ = this.vfs.currPower$.asObservable().pipe(map(val => Math.round(val * this.resolution)));
      this.rawTemp$ = this.vfs.rawTemp$.asObservable().pipe();
    })

  }

  changeMode(mode: FanMode) {
    this.vfs.sendMode(mode);
  }

  changeModeAuto(event: Event) {
    const target = event.target as HTMLElement;
    let high;

    if (event.type == "click" && target.closest('.mode-toggle')) {
      return;
    } else if (event.type == "change") {
      high = (event?.target as HTMLInputElement)?.value == 'on'
      this.changeMode(this.autoMode ? FanMode.FAN_MODE_AUTO_HIGH : FanMode.FAN_MODE_AUTO_LOW);

    } else {
      this.changeMode(this.autoMode ? FanMode.FAN_MODE_AUTO_HIGH : FanMode.FAN_MODE_AUTO_LOW)
    }

    event.preventDefault();
  }

  changePower(event: Event) {
    const target = event.target as HTMLInputElement;
    this.vfs.sendPower(parseInt(target.value, 10) / this.resolution);
  }

  chargeTargeTemp(event: Event) {
    const target = event.target as HTMLSelectElement;
    this.vfs.sendTargetTemp(parseFloat(target.value));
  }
}
