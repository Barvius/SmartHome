import { Component } from '@angular/core';
import { SensorService } from './sensor/sensor.service';

@Component({
  moduleId: module.id,
  selector: 'app',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})
export class AppComponent {
  title = 'app';
}
