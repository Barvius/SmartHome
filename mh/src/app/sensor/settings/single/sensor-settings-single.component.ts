//import { Component } from '@angular/core';
import { Sensor } from '../../sensor';
import { SensorService } from '../../sensor.service';

import { Component, Input, OnInit } from '@angular/core';
import { ActivatedRoute, ParamMap } from '@angular/router';
import { Location }                 from '@angular/common';

import 'rxjs/add/operator/switchMap';

@Component({
  moduleId: module.id,
  selector: 'sensor-settings-single',
  templateUrl: './sensor-settings-single.component.html',
  styleUrls: ['./sensor-settings-single.component.css']

})

export class SensorSettingsSingleComponent implements OnInit {
  constructor(
    private SensorService: SensorService,
    private route: ActivatedRoute,
    private location: Location,
  ) { }
  @Input() sensor: Sensor;
  S: Sensor;
  //@Input() id: string;
  ngOnInit(): void {

    this.route.paramMap
      .switchMap((params: ParamMap) => this.SensorService.getSensor(params.get('id')))
      .subscribe(data => {
        this.S = data;
        this.sensor = data;
        console.log(this.sensor);
      });
  }
  save(): void {
    this.SensorService.update(this.sensor);
    //.then(() => this.goBack());
  }
  back(): void {
    this.location.back();
  }
  // console.log(this.sensor);
}
