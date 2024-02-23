import { Component, ViewChild } from '@angular/core';
import { MatTableModule, MatTableDataSource } from '@angular/material/table';
import { MatPaginator } from '@angular/material/paginator';
import { MatSort } from '@angular/material/sort';

import { BackendService } from '../../../services/backend.service';
import { Backend } from '../../../models/backend';

@Component({
  selector: 'app-backends',
  standalone: true,
  imports: [MatTableModule, MatPaginator, MatSort],
  templateUrl: './backends.component.html',
  styleUrl: './backends.component.css'
})
export class BackendsComponent {
  items: Backend[] = [];
  selectedItem?: Backend;
  displayedColumns: string[] = ['id', 'type'];

  dataSource = new MatTableDataSource<Backend>();

  constructor(private itemService: BackendService) {}

  ngAfterViewInit(): void {
    this.getItems();
  }

  getItems(): void {
    this.itemService.get().subscribe(items => this.onItems(items));
  }

  onItems(items: Backend[])
  {
    this.dataSource.data = items;
    this.items = items;
  }

  onSelect(item: Backend): void {
    this.selectedItem = item;
  }
}
