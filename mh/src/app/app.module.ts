import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { HttpModule } from '@angular/http';
import { FormsModule }   from '@angular/forms';


import { AppComponent } from './app.component';
import { LoginComponent } from './login/login.component';
import { AuthenticationService } from './authentication/authentication.service';

import { AppRoutingModule } from './app-routing/app-routing.module';

import { SensorService } from './sensor/sensor.service';
import { SensorSettingsComponent } from './sensor/settings/sensor-settings.component';
import { SensorSettingsSingleComponent } from './sensor/settings/single/sensor-settings-single.component';
import { SensorListComponent } from './sensor/list/sensor-list.component';
import { SensorStatsComponent } from './sensor/list/stats/sensor-stats.component';

import {
  BrowserAnimationsModule,
  //NoopAnimationsModule
} from '@angular/platform-browser/animations';
import {
  //MdCardModule,
  MdToolbarModule,
  MdSidenavModule,
  MdButtonModule,
  MdIconModule,
  // MdTableModule,
  //MdDialogModule,
  MdListModule,
  MdInputModule,
  MdCheckboxModule,
  MdChipsModule,
} from '@angular/material';
//import {CdkTableModule} from '@angular/cdk';
//import 'hammerjs';
import { ChartsModule } from 'ng2-charts/ng2-charts';

@NgModule({
  declarations: [
    AppComponent,
    SensorSettingsComponent,
    SensorSettingsSingleComponent,
    SensorListComponent,
    SensorStatsComponent,
    LoginComponent,
  ],
  imports: [
    BrowserModule,
    HttpModule,
    FormsModule,
    BrowserAnimationsModule,
    AppRoutingModule,
    ChartsModule,
    //NoopAnimationsModule,
    //MdCardModule,
    MdToolbarModule,
    MdSidenavModule,
    MdButtonModule,
    MdIconModule,
  //  MdTableModule,
    //CdkTableModule,
    //MdDialogModule,
    MdListModule,
    MdInputModule,
    MdCheckboxModule,
    MdChipsModule,
  ],
  providers: [SensorService,AuthenticationService,],
  bootstrap: [AppComponent]
})
export class AppModule { }
