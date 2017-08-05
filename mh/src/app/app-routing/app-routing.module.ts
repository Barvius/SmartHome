import { NgModule }             from '@angular/core';
import { RouterModule, Routes } from '@angular/router';

import { LoginComponent }   from '../login/login.component';
import { AuthGuardService } from '../authentication/auth-guard.service';

import { SensorSettingsComponent }   from '../sensor/settings/sensor-settings.component';
import { SensorSettingsSingleComponent }   from '../sensor/settings/single/sensor-settings-single.component';
import { SensorListComponent }   from '../sensor/list/sensor-list.component';
import { SensorStatsComponent }   from '../sensor/list/stats/sensor-stats.component';


const routes: Routes = [
  // { path: '', redirectTo: '/config', pathMatch: 'full' },
  // { path: 'sensor',  component:  SensorListComponent },
  //   { path: 'config', component:  SensorSettingsComponent },
  //     { path: 'cfg/:id', component:  SensorSettingsSingleComponent },
  { path: '', redirectTo: '/sensor', pathMatch: 'full' },
  { path: 'sensor', component: SensorListComponent, canActivate: [AuthGuardService] },
  //{path: '', component: SensorListComponent},
  { path: 'settings/sensor', component: SensorSettingsComponent, canActivate: [AuthGuardService] },
  { path: 'settings/sensor/:id', component: SensorSettingsSingleComponent,canActivate: [AuthGuardService] },
  { path: 'sensor/:id', component: SensorStatsComponent,canActivate: [AuthGuardService] },

  { path: 'login', component: LoginComponent },
  { path: '**', redirectTo: '' }
  // {path: 'login', component: LoginComponent, canActivate:[SigninGuard]}
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule],
  providers: [AuthGuardService],
})
export class AppRoutingModule { }
