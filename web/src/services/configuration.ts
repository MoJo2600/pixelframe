import {
  BasicConfiguration,
  UpdateWifiConfiguration,
  WifiConfiguration
} from "@/models/configuration";
import { Service } from "@/services";

export class ConfigurationService extends Service {
  private readonly pathBasic = "configuration/basic";
  private readonly pathWifi = "configuration/wifi";

  public async getBasicConfiguration(): Promise<BasicConfiguration> {
    return (await this.httpClient.get(this.pathBasic)).data;
  }

  public async updateBasicConfiguratin(
    basicConfig: Partial<BasicConfiguration>
  ): Promise<void> {
    await this.httpClient.patch(this.pathBasic, basicConfig);
  }

  public async getWifiConfiguration(): Promise<WifiConfiguration> {
    return (await this.httpClient.get(this.pathWifi)).data;
  }

  public async updateWifiConfiguration(
    wifiConfig: UpdateWifiConfiguration
  ): Promise<void> {
    await this.httpClient.put(this.pathWifi, wifiConfig);
  }
}
