import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';

import { Observable} from 'rxjs';

import { UserService } from './user.service';
import { HsmService } from './hsm.service';

import { Dataset } from '../models/dataset';

@Injectable({
  providedIn: 'root'
})
export class DatasetService extends HsmService{

  item: string = "datasets";

  constructor(private userServiceIn: UserService, private httpIn: HttpClient) {
    super(userServiceIn, httpIn)
   }

  get(): Observable<Dataset[]> {
    return this.http_get<Dataset>(this.item);
  }
}
