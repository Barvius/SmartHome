import { Injectable } from '@angular/core';

import { Headers, Http } from '@angular/http';
import { Sensor } from './sensor';

import 'rxjs/add/operator/toPromise';

@Injectable()
export class SensorService {
  constructor(private http: Http) { }

  currentUser = JSON.parse(localStorage.getItem('currentUser'));

  private heroesUrl = 'http://192.168.1.141/api/?tooken='+this.currentUser.tooken+'&';  // URL to web api

  //sensor: Sensor[] = [];
  getSensors(): Promise<Sensor[]> {
    return this.http.get(this.heroesUrl+'sensor=get_all')
      .toPromise()
      .then(response => response.json().data as Sensor[])
      .catch(this.handleError);
  }

  getStats(id: string): Promise<string[]> {
      const url = `${this.heroesUrl}sensor=stats&id=${id}&period=minifid`;
    return this.http.get(url)
      .toPromise()
      .then(response => response.json().data.data as string[])
      .catch(this.handleError);
  }

  getSensorValue(id: string): Promise<number> {
    const url = `${this.heroesUrl}sensor=stats&id=${id}&period=now`;
    return this.http.get(url)
      .toPromise()
      .then(response => response.json().data as Sensor)
      .catch(this.handleError);
  }

  getSensor(id: string): Promise<Sensor> {
    const url = `${this.heroesUrl}sensor=get&id=${id}`;
    return this.http.get(url)
      .toPromise()
      .then(response => response.json().data as Sensor)
      .catch(this.handleError);
  }

  private handleError(error: any): Promise<any> {
    console.error('An error occurred', error); // for demo purposes only
    return Promise.reject(error.message || error);
  }

  private headers = new Headers({'Content-Type': 'application/json'});

  update(sensor: Sensor): Promise<Sensor> {
    const url = `${this.heroesUrl}sensor=update&id=${sensor.id}`;

      return this.http
        .post(url, JSON.stringify(sensor), {headers: this.headers})
        .toPromise()
        .then(res => res.json().data as Sensor)
        .catch(this.handleError);
  }
}
