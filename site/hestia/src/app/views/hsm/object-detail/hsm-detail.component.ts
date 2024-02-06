import { Component, Input } from '@angular/core';
import { NgFor, NgIf } from '@angular/common';
import { FormsModule } from '@angular/forms';

import {HsmObject} from '../../../models/hsm-object'

@Component({
  selector: 'app-hsm-detail',
  standalone: true,
  imports: [NgFor, NgIf, FormsModule],
  templateUrl: './hsm-detail.component.html',
  styleUrl: './hsm-detail.component.css'
})

export class HsmDetailComponent {
  @Input() object?: HsmObject;
}
