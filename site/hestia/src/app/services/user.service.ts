import { Injectable } from '@angular/core';
import { Observable, of } from 'rxjs';

import { HttpClient, HttpHeaders, HttpClientModule } from '@angular/common/http';

import { User } from '../models/user';

@Injectable({
  providedIn: 'root'
})
export class UserService {

  loggedInUser?: User;
  endpoint: string = "http://localhost:8080/api/v1/login"

  constructor(private http: HttpClient) { }

  login(username:string, password:string): Observable<User>
  {
    console.log("hitting: " + this.endpoint);
    return this.http.get<User>(this.endpoint);
  }
}
