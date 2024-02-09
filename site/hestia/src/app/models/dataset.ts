import { HsmObject } from "./hsm-object";

export interface Dataset {
    id: string;
    name?: string;
    objects: HsmObject[];
  }