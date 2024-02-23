import { Component, Input } from '@angular/core';
import { NgIf, NgFor } from '@angular/common';

import { MatToolbarModule } from '@angular/material/toolbar';
import {MatIconModule} from '@angular/material/icon';
import {MatButtonModule} from '@angular/material/button';
import {MatMenuModule} from '@angular/material/menu';

import { UserService } from '../../services/user.service';
import { LeftNavService } from '../../services/left-nav.service';
import { User } from '../../models/user';

@Component({
  selector: 'app-top-bar',
  standalone: true,
  imports: [NgIf, NgFor, MatToolbarModule, MatIconModule, MatButtonModule, MatMenuModule],
  templateUrl: './top-bar.component.html',
  styleUrl: './top-bar.component.css'
})
export class TopBarComponent {
  user?:User;
  username:string = "";
  @Input() title:string = "";

  constructor(private userService: UserService,
    private leftNavService: LeftNavService) {}

  ngOnInit(): void {
    this.userService.isLogged.subscribe(logged_in => {
      if (logged_in)
      {
        this.user = this.userService.loggedInUser;
        if (this.user)
        {
          this.username = this.user.name;
        }
      }
    });
  }

  getCategories() 
  {
    return this.leftNavService.categories;
  }

  onCategory(category_name:string)
  {
    this.leftNavService.onCategory(category_name);
  }
}
