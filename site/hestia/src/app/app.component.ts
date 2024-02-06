import { Component } from '@angular/core';
import { RouterOutlet, RouterModule } from '@angular/router';

import { UserNavComponent } from './views/user-nav/user-nav.component';
import { HsmObjectComponent } from './views/hsm/objects/hsm-object.component';
import { HsmDetailComponent } from './views/hsm/object-detail/hsm-detail.component';

import { AdminNavComponent } from './views/admin-nav/admin-nav.component';
import { LandingComponent } from './views/landing/landing.component';

@Component({
  selector: 'app-root',
  standalone: true,
  imports: [RouterOutlet,
            RouterModule,
            UserNavComponent,
            AdminNavComponent,
            HsmDetailComponent,
            HsmObjectComponent,
            LandingComponent],
  templateUrl: './app.component.html',
  styleUrl: './app.component.css'
})
export class AppComponent {
  title = 'hestia';
}
