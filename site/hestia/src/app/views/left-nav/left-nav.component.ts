import { Component } from '@angular/core';
import { NgFor, NgIf } from '@angular/common';

import { RouterOutlet, RouterModule } from '@angular/router';

import {MatSidenavModule} from '@angular/material/sidenav';
import {MatListModule} from '@angular/material/list';

import { LeftNavService } from '../../services/left-nav.service';

@Component({
  selector: 'app-left-nav',
  standalone: true,
  imports: [ MatSidenavModule, MatListModule, RouterOutlet, RouterModule,],
  templateUrl: './left-nav.component.html',
  styleUrl: './left-nav.component.css'
})
export class LeftNavComponent {

  constructor(private leftNavService : LeftNavService)
  {
  }

  getOptions()
  {
    return this.leftNavService.getOptions();
  }

}
