import { Component } from '@angular/core';
import { VentilationFanComponent } from './ventilation-fan/ventilation-fan.component';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss'],
  imports: [VentilationFanComponent],
})
export class AppComponent {}
