import { Service } from "@/services";

export class VisualsService extends Service {
  public async showVisual(name: string): Promise<void> {
    await this.httpClient.get(`show/visuals?v=${name}`);
  }
}
