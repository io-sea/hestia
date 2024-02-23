import { Injectable } from '@angular/core';

import { LeftNavCategory, LeftNavOption } from '../models/left-nav-options';

@Injectable({
  providedIn: 'root'
})
export class LeftNavService {

  active_category:string = "Content";
  active_option:string = "Datasets";
  categories: LeftNavCategory[] = [{name : "Content", 
                                    options : [{name : "Datasets", route : "datasets"},
                                               {name : "Actions", route : "actions"}]},
                                   {name : "Admin", 
                                    options : [{name : "Nodes", route : "nodes"},
                                                {name : "Backends", route : "backends"},
                                                {name : "Tiers", route : "tiers"},
                                              ]}
                                                ];

  constructor() { }

  onCategory(selection: string)
  {
    this.active_category = selection;
    for (let category of this.categories)
    {
      if (category.name == this.active_category)
      {
        this.active_option = category.options[0].name;
      }
    }
  }

  getOptions()
  {
    for (let category of this.categories)
    {
      if (category.name == this.active_category)
      {
        return category.options;
      }
    }
    return [];
  }


}
