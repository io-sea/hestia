import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';

import { Observable} from 'rxjs';

import { UserService } from './user.service';
import { HsmService } from './hsm.service';

import { Tier } from '../models/tier';

@Injectable({
  providedIn: 'root'
})
export class TierService extends HsmService{

  item: string = "tiers";

  constructor(private userServiceIn: UserService, private httpIn: HttpClient) {
    super(userServiceIn, httpIn)
   }

  get(): Observable<Tier[]> {
    return this.http_get<Tier>(this.item);
  }
}