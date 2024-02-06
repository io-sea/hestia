export interface Extent {
  offset: number;
  length: number;
}

export interface HsmObject {
    id: string;
    name?: string;
    extents: Extent[];
  }