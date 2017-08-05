import { Component, OnInit } from '@angular/core';

import { SensorService } from '../sensor.service';
import { Sensor } from '../sensor';

@Component({
  moduleId: module.id,
  selector: 'sensor-settings',
  templateUrl: './sensor-settings.component.html',
  styleUrls: [ './sensor-settings.component.css' ]
})

//export class SensorComponent {
export class SensorSettingsComponent implements OnInit {

  sensors: Sensor[] = [];
  constructor(private SensorService: SensorService) { }

  ngOnInit(): void {

    this.SensorService.getSensors().then(data => {
      this.sensors = data;
      console.log(this.sensors);
    });
    this.SensorService.getSensors().then(data => {
      console.log(data);
    });
  }



}
