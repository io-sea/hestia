import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';

import { Observable} from 'rxjs';

import { UserService } from './user.service';
import { HsmService } from './hsm.service';

import { Node } from '../models/node';

@Injectable({
  providedIn: 'root'
})
export class NodeService extends HsmService{

  item: string = "nodes";

  constructor(private userServiceIn: UserService, private httpIn: HttpClient) {
    super(userServiceIn, httpIn)
   }

  get(): Observable<Node[]> {
    return this.http_get<Node>(this.item);
  }
}