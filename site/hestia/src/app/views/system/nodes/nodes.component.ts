import { Component, ViewChild } from '@angular/core';
import { MatTableModule, MatTableDataSource } from '@angular/material/table';
import { MatPaginator } from '@angular/material/paginator';
import { MatSort } from '@angular/material/sort';

import { NodeService } from '../../../services/node.service';
import { Node } from '../../../models/node';


@Component({
  selector: 'app-nodes',
  standalone: true,
  imports: [MatTableModule, MatPaginator, MatSort],
  templateUrl: './nodes.component.html',
  styleUrl: './nodes.component.css'
})
export class NodesComponent {
  items: Node[] = [];
  selectedItem?: Node;
  displayedColumns: string[] = ['id', 'name', 'host'];

  dataSource = new MatTableDataSource<Node>();

  constructor(private itemService: NodeService) {}

  ngAfterViewInit(): void {
    this.getItems();
  }

  getItems(): void {
    this.itemService.get().subscribe(items => this.onItems(items));
  }

  onItems(items: Node[])
  {
    this.dataSource.data = items;
    this.items = items;
  }

  onSelect(item: Node): void {
    this.selectedItem = item;
  }
}
