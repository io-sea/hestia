import { Component } from '@angular/core';
import { NgFor, NgIf } from '@angular/common';

import { HsmObjectComponent } from './views/hsm/objects/hsm-object.component';
import { HsmDetailComponent } from './views/hsm/object-detail/hsm-detail.component';

import { LandingComponent } from './views/landing/landing.component';
import { TopBarComponent } from './views/top-bar/top-bar.component';
import { LeftNavComponent } from './views/left-nav/left-nav.component';

import { UserService } from './services/user.service';
import { User } from './models/user';

@Component({
  selector: 'app-root',
  standalone: true,
  imports: [NgIf, 
            LeftNavComponent,
            HsmDetailComponent,
            HsmObjectComponent,
            LandingComponent,
            TopBarComponent
          ],
  templateUrl: './app.component.html',
  styleUrl: './app.component.css'
})

export class AppComponent {
  title = 'hestia';
  user?:User;
  username:string = "";

  constructor(private userService: UserService) {}

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

}
