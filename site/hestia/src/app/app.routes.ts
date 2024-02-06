import { Routes } from '@angular/router';

import { HsmObjectComponent } from './views/hsm/objects/hsm-object.component';
import { LandingComponent } from './views/landing/landing.component';
import { UserNavComponent } from './views/user-nav/user-nav.component';
import { AdminNavComponent } from './views/admin-nav/admin-nav.component';

export const routes: Routes = [
    { path: '', redirectTo: '/landing', pathMatch: 'full' },
    { path: 'user', component: UserNavComponent },
    { path: 'landing', component: LandingComponent }
];
