export interface UserToken {
  value: string
}

export interface User {
    id: number;
    name: string;
    tokens: UserToken[];
  }