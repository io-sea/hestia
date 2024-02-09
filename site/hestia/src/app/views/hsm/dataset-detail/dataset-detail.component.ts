import { Component, Input } from '@angular/core';
import { NgFor, NgIf } from '@angular/common';
import { FormsModule } from '@angular/forms';

import { Dataset } from '../../../models/dataset';

@Component({
  selector: 'app-dataset-detail',
  standalone: true,
  imports: [NgFor, NgIf, FormsModule],
  templateUrl: './dataset-detail.component.html',
  styleUrl: './dataset-detail.component.css'
})

export class DatasetDetailComponent {
  @Input() dataset?: Dataset;
}
