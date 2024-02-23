import { Injectable } from '@angular/core';
import { Observable, of, catchError,  throwError} from 'rxjs';

import { HttpClient, HttpErrorResponse, HttpHeaders, HttpClientModule } from '@angular/common/http';

import { UserService } from './user.service';

@Injectable({
  providedIn: 'root'
})
export class HsmService {

  endpoint: string = "http://localhost:8080/api/v1/"

  constructor(private userService: UserService, private http: HttpClient) { }

  http_get<T>(item:string, query:string=""): Observable<T[]> {

    let token = this.userService.loggedInUser?.tokens[0].value ?? "";
    let headers = new HttpHeaders({ 'Authorization': token });

    let url = this.endpoint + item + query;
    return this.http.get<T[]>(url, { headers: headers }).pipe(
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
