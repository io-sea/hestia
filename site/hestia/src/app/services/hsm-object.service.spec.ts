import { TestBed } from '@angular/core/testing';

import { HsmObjectService } from './hsm-object.service';

describe('HsmObjectService', () => {
  let service: HsmObjectService;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    service = TestBed.inject(HsmObjectService);
  });

  it('should be created', () => {
    expect(service).toBeTruthy();
  });
});
