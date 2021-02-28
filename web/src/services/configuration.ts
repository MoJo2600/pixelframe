import {
  BasicConfiguration,
  MqttConfiguration,
  UpdateMqttConfiguration,
  UpdateWifiConfiguration,
  WifiConfiguration
} from "@/models/configuration";
import { Service } from "@/services";

export class ConfigurationService extends Service {
  private readonly pathBasic = "configuration/basic";
  private readonly pathWifi = "configuration/wifi";
  private readonly pathMqtt = "configuration/mqtt";

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

  public async getMqttConfiguration(): Promise<MqttConfiguration> {
    return (await this.httpClient.get(this.pathMqtt)).data;
  }

  public async updateMqttConfiguration(
    mqttConfig: UpdateMqttConfiguration
  ): Promise<void> {
    await this.httpClient.put(this.pathMqtt, mqttConfig);
  }
}
