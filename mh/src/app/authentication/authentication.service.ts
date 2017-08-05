import { Injectable } from '@angular/core';
import { Http, Headers, Response } from '@angular/http';
import { Observable } from 'rxjs/Observable';
import 'rxjs/add/operator/map'

@Injectable()
export class AuthenticationService {
    constructor(private http: Http) { }

    login(username: string, password: string) {
        return this.http.post('http://192.168.1.141/api/user.php?act=login', JSON.stringify({ username: username, password: password }))
            .map((response: Response) => {
                // login successful if there's a jwt token in the response
                let user = response.json().data;
                console.log(user);
                if (user && user.tooken) {
                    // store user details and jwt token in local storage to keep user logged in between page refreshes
                    localStorage.setItem('currentUser', JSON.stringify(user));
                }
            });
    }
    CheckLogin():Observable<boolean> {
        let currentUser = JSON.parse(localStorage.getItem('currentUser'));
        return this.http.post('http://192.168.1.141/api/user.php?act=check', JSON.stringify({ username: currentUser.username, tooken: currentUser.tooken }))
            .map((response: Response) => {
                // login successful if there's a jwt token in the response
                let user = response.json().data;
                if (user) {
                    return true;
                }
                return false;
            });
    }

    logout() {
        // remove user from local storage to log user out
        localStorage.removeItem('currentUser');
    }
}
