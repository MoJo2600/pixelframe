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

  public async updateBasicConfiguration(
    basicConfig: Partial<BasicConfiguration>
  ): Promise<void> {
    // We have to set the content type because the remote api can't parse the charset after the content-type
    await this.httpClient.patch(this.pathBasic, basicConfig, { headers: { "Content-Type": "application/json" } });
  }

  public async getWifiConfiguration(): Promise<WifiConfiguration> {
    return (await this.httpClient.get(this.pathWifi)).data;
  }

  public async updateWifiConfiguration(
    wifiConfig: UpdateWifiConfiguration
  ): Promise<void> {
    // We have to set the content type because the remote api can't parse the charset after the content-type
    await this.httpClient.put(this.pathWifi, wifiConfig, { headers: { "Content-Type": "application/json" } });
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
