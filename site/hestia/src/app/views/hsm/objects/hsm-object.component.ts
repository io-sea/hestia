import { Component } from '@angular/core';
import { NgFor, NgIf } from '@angular/common';

import { HsmObjectService } from '../../../services/hsm-object.service';
import { HsmObject} from '../../../models/hsm-object';
import { HsmDetailComponent } from '../object-detail/hsm-detail.component';

@Component({
  selector: 'app-hsm-object',
  standalone: true,
  imports: [NgFor, HsmDetailComponent],
  templateUrl: './hsm-object.component.html',
  styleUrl: './hsm-object.component.css'
})
export class HsmObjectComponent {
  objects: HsmObject[] = [];
  selectedObject?: HsmObject;

  constructor(private hsmObjectService: HsmObjectService) {}

  ngOnInit(): void {
    this.getObjects();
  }

  getObjects(): void {
    this.hsmObjectService.getObjects().subscribe(objects => this.objects = objects);
  }

  onSelect(object: HsmObject): void {
    this.selectedObject = object;
  }
}
