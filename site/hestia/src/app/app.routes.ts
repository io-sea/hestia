import { Routes } from '@angular/router';

import { DatasetsComponent } from './views/hsm/datasets/datasets.component';
import { ActionsComponent } from './views/hsm/actions/actions.component';
import { TiersComponent } from './views/system/tiers/tiers.component';
import { NodesComponent  } from './views/system/nodes/nodes.component';
import { BackendsComponent } from './views/system/backends/backends.component';

export const routes: Routes = [
    { path: 'datasets', component: DatasetsComponent },
    { path: 'actions', component: ActionsComponent },
    { path: 'tiers', component: TiersComponent },
    { path: 'nodes', component: NodesComponent },
    { path: 'backends', component: BackendsComponent },
];
