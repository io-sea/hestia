import { Component } from '@angular/core';
import { Observable } from 'rxjs';
import { FormsModule } from '@angular/forms';
import { Router } from '@angular/router';

import { UserService } from '../../services/user.service';
import { User } from '../../models/user';
import { LoginUser } from './login-user';

@Component({
  selector: 'app-landing',
  standalone: true,
  imports: [FormsModule],
  templateUrl: './landing.component.html',
  styleUrl: './landing.component.css'
})

export class LandingComponent {

  loginUser: LoginUser = {password:"", name:""};

  constructor(private userService: UserService, private router: Router) {}

  login()
  {
      return this.userService.login(this.loginUser.name, this.loginUser.password).subscribe(() => this.onLoggedIn());
  }

  onLoggedIn()
  {
    this.router.navigateByUrl("/user");
  }

}
