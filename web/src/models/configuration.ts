export interface BasicConfiguration {
  brightness: number;
  timezone: string;
}

export interface WifiConfiguration {
  ssid: string;
}

export interface UpdateWifiConfiguration extends WifiConfiguration {
  password: string;
}

export interface MqttConfiguration {
  host: string;
  port: number;
  user: string;
  baseTopic: string;
}

export interface UpdateMqttConfiguration extends MqttConfiguration {
  password: string;
}
