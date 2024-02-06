import { Injectable } from '@angular/core';
import { Observable, of } from 'rxjs';

import { Dataset } from '../models/dataset';
import { UserService } from './user.service';

@Injectable({
  providedIn: 'root'
})
export class DatasetService {

  constructor(private userService: UserService) { }

  get(): Observable<Dataset[]> {
    const datasets = of([
      { id: "12", name: 'my_bucket', objects: [] },
      { id: "13", name: 'bucket123', objects: []  },
      { id: "14", name: 'exp5', objects: [] },
      { id: "15", name: '1234567', objects: []  }
    ]);
    return datasets;
  }
}
