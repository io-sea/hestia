import { Component } from '@angular/core';
import { NgFor, NgIf } from '@angular/common';
import { RouterModule } from '@angular/router';

import { DatasetsComponent } from '../hsm/datasets/datasets.component';
import { ActionsComponent } from '../hsm/actions/actions.component';
import { UserService } from '../../services/user.service';
import { User } from '../../models/user';

@Component({
  selector: 'app-user-nav',
  standalone: true,
  imports: [NgIf, RouterModule,
    DatasetsComponent,
    ActionsComponent],
  templateUrl: './user-nav.component.html',
  styleUrl: './user-nav.component.css'
})

export class UserNavComponent {
  user?: User;

  constructor(private userService: UserService) {}

  ngOnInit(): void {
    this.user = this.userService.loggedInUser;
    console.log(this.user);
  }
}
