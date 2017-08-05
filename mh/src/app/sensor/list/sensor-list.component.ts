import { Component, OnInit } from '@angular/core';

import { SensorService } from '../sensor.service';
import { Sensor } from '../sensor';

@Component({
  moduleId: module.id,
  selector: 'sensor-list',
  templateUrl: './sensor-list.component.html',
  styleUrls: [ './sensor-list.component.css' ]
})

//export class SensorComponent {
export class SensorListComponent implements OnInit {

  sensors: Sensor[] = [];
  constructor(private SensorService: SensorService) { }

  ngOnInit(): void {

    this.SensorService.getSensors().then(data => {
      this.sensors = data;
    });

  }



}
