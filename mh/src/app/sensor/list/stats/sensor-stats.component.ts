import { Component, OnInit } from '@angular/core';


import { Headers, Http } from '@angular/http';
import { SensorService } from '../../sensor.service';
import { Sensor } from '../../sensor';

// import { Component, Input, OnInit } from '@angular/core';
import { ActivatedRoute, ParamMap } from '@angular/router';
import { Location }                 from '@angular/common';

import 'rxjs/add/operator/switchMap';

@Component({
  moduleId: module.id,
  selector: 'sensor-stats',
  templateUrl: './sensor-stats.component.html',
  styleUrls: [ './sensor-stats.component.css' ]
})

//export class SensorComponent {
export class SensorStatsComponent {
  // lineChart
    constructor(
      private http: Http,
      private SensorService: SensorService,
      private route: ActivatedRoute,
      private location: Location,
    ) { }
    public ChartData:Array<number>[] = [];

    public lineChartLabels:Array<any>=[];
    public lineChartData:Array<any> = [
    {data: [], label: 'min'},
    {data: [], label: 'avg'},
    {data: [], label: 'max'},
  ];
    sensor: Sensor;
    ngOnInit(): void {
      this.route.paramMap
        .switchMap((params: ParamMap) => this.SensorService.getSensor(params.get('id')))
        .subscribe(data => {
          this.sensor = data;
          console.log(this.sensor);
        });

      this.route.paramMap
        .switchMap((params: ParamMap) => this.SensorService.getStats(params.get('id')))
        .subscribe(data => {
          // this.lineChartData = {data: new Array(data.length), label: 'a'};
          var a = data['min'];
          for (let row of a) {
            if(row){
              this.lineChartLabels.push(row[0]);
              this.lineChartData[0].data.push(parseFloat(row[1]));
            }
          }
          var a = data['avg'];
          for (let row of a) {
            if(row){
              //this.lineChartLabels.push(row[0]);
              this.lineChartData[1].data.push(parseFloat(row[1]));
            }
          }
          var a = data['max'];
          for (let row of a) {
            if(row){
              //this.lineChartLabels.push(row[0]);
              this.lineChartData[2].data.push(parseFloat(row[1]));
            }
          }
          console.log(this.ChartData);
        });
    }

   public lineChartOptions:any = {
     responsive: true,
     maintainAspectRatio: false,
     legend: {
            display: true,
          },

   };

   public lineChartLegend:boolean = true;
   public lineChartType:string = 'line';


   // events
   public chartClicked(e:any):void {
     console.log(e);
   }

   public chartHovered(e:any):void {
     console.log(e);
   }
}
