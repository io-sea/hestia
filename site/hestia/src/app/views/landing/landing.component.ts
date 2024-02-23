import { Component } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { Router } from '@angular/router';

import {MatIconModule} from '@angular/material/icon';
import {MatButtonModule} from '@angular/material/button';
import {MatInputModule} from '@angular/material/input';
import {MatFormFieldModule} from '@angular/material/form-field';
import {MatCardModule} from '@angular/material/card';

import { Observable } from 'rxjs';

import { UserService } from '../../services/user.service';
import { User } from '../../models/user';
import { LoginUser } from './login-user';

@Component({
  selector: 'app-landing',
  standalone: true,
  imports: [FormsModule, MatCardModule, MatFormFieldModule, MatInputModule, MatButtonModule, MatIconModule],
  templateUrl: './landing.component.html',
  styleUrl: './landing.component.css'
})

export class LandingComponent {

  loginUser: LoginUser = {password:"my_user_pass", name:"hestia_default_user"};

  constructor(private userService: UserService, private router: Router) {}

  ngOnInit(): void {
    this.login();
  }

  login()
  {
      return this.userService.login(this.loginUser.name, this.loginUser.password).subscribe(() => this.onLoggedIn());
  }

  onLoggedIn()
  {
    //this.router.navigateByUrl("/user");
  }

}
