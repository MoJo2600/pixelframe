export enum ImageType {
  File = "file",
  Directory = "dir"
}

export interface Image {
  type: ImageType;
  size: number;
  name: string;
}
