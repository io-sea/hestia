import { Component } from '@angular/core';
import { RouterModule } from '@angular/router';

import { DatasetsComponent } from '../hsm/datasets/datasets.component';
import { ActionsComponent } from '../hsm/actions/actions.component';
import { User } from '../../models/user';

@Component({
  selector: 'app-user-nav',
  standalone: true,
  imports: [RouterModule,
    DatasetsComponent,
    ActionsComponent],
  templateUrl: './user-nav.component.html',
  styleUrl: './user-nav.component.css'
})

export class UserNavComponent {
  user: User = {id:1, name: "my_user"};
}
