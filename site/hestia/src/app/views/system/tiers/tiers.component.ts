import { Component, ViewChild } from '@angular/core';
import { MatTableModule, MatTableDataSource } from '@angular/material/table';
import { MatPaginator } from '@angular/material/paginator';
import { MatSort } from '@angular/material/sort';

import { TierService } from '../../../services/tier.service';
import { Tier } from '../../../models/tier';

@Component({
  selector: 'app-tiers',
  standalone: true,
  imports: [MatTableModule, MatPaginator, MatSort],
  templateUrl: './tiers.component.html',
  styleUrl: './tiers.component.css'
})
export class TiersComponent {
  items: Tier[] = [];
  selectedItem?: Tier;
  displayedColumns: string[] = ['id', 'priority'];

  dataSource = new MatTableDataSource<Tier>();

  constructor(private itemService: TierService) {}

  ngAfterViewInit(): void {
    this.getItems();
  }

  getItems(): void {
    this.itemService.get().subscribe(items => this.onItems(items));
  }

  onItems(items: Tier[])
  {
    this.dataSource.data = items;
    this.items = items;
  }

  onSelect(item: Tier): void {
    this.selectedItem = item;
  }
}
