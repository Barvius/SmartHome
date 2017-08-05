import { Injectable } from '@angular/core';
import { Router, CanActivate, CanActivateChild, ActivatedRouteSnapshot, RouterStateSnapshot } from '@angular/router';
import { AuthenticationService } from '../authentication/authentication.service';
import { Observable } from 'rxjs/Observable';

@Injectable()

export class AuthGuardService implements CanActivate, CanActivateChild {

  constructor(
    private Authentication: AuthenticationService,
    private router: Router
  ) { }

  canActivate(): Observable<boolean> | Promise<boolean> | boolean {

    // if (this.Authentication.CheckLogin()) {
    if (localStorage.getItem('currentUser')) {
      return true;
    }

    this.router.navigate(['/login']);
    return false;
  }

  canActivateChild() {
    return this.canActivate();
  }

}
