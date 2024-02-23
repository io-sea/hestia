import { Component, Input } from '@angular/core';
import { NgFor, NgIf } from '@angular/common';

import { HsmObjectService } from '../../../services/hsm-object.service';
import { HsmObject} from '../../../models/hsm-object';
import { HsmDetailComponent } from '../object-detail/hsm-detail.component';
import { Dataset } from '../../../models/dataset';

@Component({
  selector: 'app-hsm-object',
  standalone: true,
  imports: [NgFor, HsmDetailComponent],
  templateUrl: './hsm-object.component.html',
  styleUrl: './hsm-object.component.css'
})
export class HsmObjectComponent {
  objects: HsmObject[] = [];

  @Input() dataset?: Dataset;
  selectedObject?: HsmObject;

  constructor(private hsmObjectService: HsmObjectService) {}

  ngOnInit(): void {
    this.getObjects();
  }

  getObjects(): void {
    if(this.dataset)
    {
      this.hsmObjectService.getObjects(this.dataset).subscribe(objects => this.objects = objects);
    }
    
  }

  onSelect(object: HsmObject): void {
    this.selectedObject = object;
  }
}
