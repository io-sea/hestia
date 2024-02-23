import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';

import { Observable} from 'rxjs';

import { UserService } from './user.service';
import { HsmService } from './hsm.service';

import { HsmAction } from '../models/hsm_action';

@Injectable({
  providedIn: 'root'
})
export class ActionService extends HsmService{

  item: string = "actions";

  constructor(private userServiceIn: UserService, private httpIn: HttpClient) {
    super(userServiceIn, httpIn)
   }

  get(): Observable<HsmAction[]> {
    return this.http_get<HsmAction>(this.item);
  }
}