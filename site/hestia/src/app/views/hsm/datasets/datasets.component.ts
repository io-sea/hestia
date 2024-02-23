import { Component } from '@angular/core';
import { NgFor, NgIf, SlicePipe } from '@angular/common';
import {MatSidenavModule} from '@angular/material/sidenav';
import {MatListModule} from '@angular/material/list';

import { Dataset } from '../../../models/dataset';
import { DatasetService } from '../../../services/dataset.service';
import { DatasetDetailComponent } from '../dataset-detail/dataset-detail.component';

@Component({
  selector: 'app-datasets',
  standalone: true,
  imports: [NgFor, NgIf, SlicePipe, DatasetDetailComponent, MatSidenavModule, MatListModule],
  templateUrl: './datasets.component.html',
  styleUrl: './datasets.component.css'
})
export class DatasetsComponent {
  datasets: Dataset[] = [];
  selectedDataset?: Dataset;

  constructor(private datasetService: DatasetService) {}

  ngOnInit(): void {
    this.getDatasets();
  }

  getDatasets(): void {
    this.datasetService.get().subscribe(datasets => this.datasets = datasets);
  }

  onSelect(dataset: Dataset): void {
    this.selectedDataset = dataset;
  }
}
