import { TestBed } from '@angular/core/testing';

import { HsmService } from './hsm.service';

describe('HsmService', () => {
  let service: HsmService;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    service = TestBed.inject(HsmService);
  });

  it('should be created', () => {
    expect(service).toBeTruthy();
  });
});
