import { Service } from "@/services";

export class FramesService extends Service {
  public async showFrame(name: string): Promise<void> {
    await this.httpClient.get(`show/${name}`);
  }
}
