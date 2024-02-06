import { Component } from '@angular/core';
import { NgFor, NgIf } from '@angular/common';

import { Dataset } from '../../../models/dataset';
import { DatasetService } from '../../../services/dataset.service';
import { DatasetDetailComponent } from '../dataset-detail/dataset-detail.component';

@Component({
  selector: 'app-datasets',
  standalone: true,
  imports: [NgFor, NgIf, DatasetDetailComponent],
  templateUrl: './datasets.component.html',
  styleUrl: './datasets.component.css'
})
export class DatasetsComponent {
  datasets: Dataset[] = [];
  selectedDataset?: Dataset;

  constructor(private datasetService: DatasetService) {}

  ngOnInit(): void {
    this.getObjects();
  }

  getObjects(): void {
    this.datasetService.get().subscribe(datasets => this.datasets = datasets);
  }

  onSelect(dataset: Dataset): void {
    this.selectedDataset = dataset;
  }
}
