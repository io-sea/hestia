import { Injectable } from '@angular/core';
import { Observable, of, catchError,  throwError} from 'rxjs';

import { HttpClient, HttpErrorResponse, HttpHeaders, HttpClientModule } from '@angular/common/http';

import { Dataset } from '../models/dataset';
import { UserService } from './user.service';

@Injectable({
  providedIn: 'root'
})
export class DatasetService {

  endpoint: string = "http://localhost:8080/api/v1/datasets"

  constructor(private userService: UserService, private http: HttpClient) { }

  get(): Observable<Dataset[]> {

    let token = this.userService.loggedInUser?.tokens[0].value ?? "";
    let headers = new HttpHeaders({ 'Authorization': token });

    return this.http.get<Dataset[]>(this.endpoint, { headers: headers }).pipe(
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
