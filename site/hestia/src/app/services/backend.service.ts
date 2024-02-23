import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';

import { Observable} from 'rxjs';

import { UserService } from './user.service';
import { HsmService } from './hsm.service';

import { Backend } from '../models/backend';

@Injectable({
  providedIn: 'root'
})
export class BackendService extends HsmService{

  item: string = "object_store_backends";

  constructor(private userServiceIn: UserService, private httpIn: HttpClient) {
    super(userServiceIn, httpIn)
   }

  get(): Observable<Backend[]> {
    return this.http_get<Backend>(this.item);
  }
}