import { Injectable } from '@angular/core';
import { Observable, of, tap, throwError, catchError } from 'rxjs';

import { HttpClient, HttpErrorResponse, HttpHeaders, HttpClientModule } from '@angular/common/http';

import { User } from '../models/user';

@Injectable({
  providedIn: 'root'
})
export class UserService {

  loggedInUser?: User;
  endpoint: string = "http://localhost:8080/api/v1/login"

  constructor(private http: HttpClient) { }

  get_logged_in_user()
  {
    
  }

  login(username:string, password:string): Observable<User>
  {
    let body = new URLSearchParams();
    body.set('user', username);
    body.set('password', password);

    let headers = new HttpHeaders({ 'Content-Type': 'application/x-www-form-urlencoded' });

    console.log("hitting: " + this.endpoint);
    return this.http.post<User>(this.endpoint, body.toString(), { headers: headers }).pipe(
        tap(user => this.loggedInUser = user),
        catchError(this.handleError));
  }

  handleError(error: HttpErrorResponse)
  {
    if (error.status === 0) {
      // A client-side or network error occurred. Handle it accordingly.
      console.error('An error occurred:', error.error);
    } else {
      // The backend returned an unsuccessful response code.
      // The response body may contain clues as to what went wrong.
      console.error(
        `Backend returned code ${error.status}, body was: `, error.error);
    }
    // Return an observable with a user-facing error message.
    return throwError(() => new Error('Something bad happened; please try again later.'));
  }
}
