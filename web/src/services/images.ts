import { Image, ImageType } from "@/models/images";
import { Service } from "@/services";

export class ImagesService extends Service {
  public async listImages(): Promise<Image[]> {
    const imagesDirectoryContent = (await this.httpClient.get("images"))
      .data as Image[];

    return imagesDirectoryContent.filter(i => i.type === ImageType.File);
  }

  public async uploadImage(file: File): Promise<void> {
    const formData = new FormData();
    formData.append("file", file);

    await this.httpClient.post("images", formData);
  }

  public async playImage(name: string): Promise<void> {
    await this.httpClient.get(`play?image=${name}`);
  }

  public async deleteImage(name: string): Promise<void> {
    // TODO: implement
  }
}
