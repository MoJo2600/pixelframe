import { BasicConfiguration } from "@/models/configuration";
import { Service } from "@/services";

export class ConfigurationService extends Service {
  private readonly pathBasic = "configuration/basic";

  public async getBasicConfiguration(): Promise<BasicConfiguration> {
    return (await this.httpClient.get(this.pathBasic)).data;
  }

  public async updateBasicConfiguratin(
    basicConfig: Partial<BasicConfiguration>
  ): Promise<void> {
    await this.httpClient.patch(this.pathBasic, basicConfig);
  }
}
