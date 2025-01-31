export interface TrackedPoint {
  id: number;
  x: number;
  y: number;
  bearing: number;
  velocity?: [number, number];
}
