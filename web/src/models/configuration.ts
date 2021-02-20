export interface BasicConfiguration {
  brightness: number;
  timezone: string;
}

export interface WifiConfiguration {
  ssid: string;
}

export interface UpdateWifiConfiguration {
  ssid: string;
  password: string;
}
