import { Injectable } from '@angular/core';
import { Observable, of } from 'rxjs';

import { HsmObject } from '../models/hsm-object';
import { UserService } from './user.service';

@Injectable({
  providedIn: 'root'
})
export class HsmObjectService {

  constructor(private userService: UserService) { }

  getObjects(): Observable<HsmObject[]> {
    const objects = of([
      { id: "12", name: 'Dr. Nice', extents: [] },
      { id: "13", name: 'Bombasto', extents: [] },
      { id: "14", name: 'Celeritas', extents: [] },
      { id: "15", name: 'Magneta', extents: [] }
    ]);
    return objects;
  }
}
