import { Wifi } from "@/models/environment";
import { Service } from "@/services";

export class EnvironmentService extends Service {
  public async getAvailableWifis(): Promise<Wifi[]> {
    return (await this.httpClient.get("environment/wifis")).data;
  }
}
