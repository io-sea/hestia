import { ComponentFixture, TestBed } from '@angular/core/testing';

import { BackendsComponent } from './backends.component';

describe('BackendsComponent', () => {
  let component: BackendsComponent;
  let fixture: ComponentFixture<BackendsComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [BackendsComponent]
    })
    .compileComponents();
    
    fixture = TestBed.createComponent(BackendsComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
