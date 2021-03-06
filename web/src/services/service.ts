import Axios, { AxiosInstance } from "axios";

export abstract class Service {
  protected readonly baseUrl: string;
  protected readonly httpClient: AxiosInstance;

  constructor() {
    this.baseUrl = process.env.VUE_APP_API_BASE_URL || window.location.origin;

    console.log(`API: ${this.baseUrl}`);

    this.httpClient = Axios.create({
      baseURL: `${this.baseUrl}/api`,
      timeout: process.env.VUE_APP_HTTP_CLIENT_TIMEOUT
        ? Number(process.env.VUE_APP_HTTP_CLIENT_TIMEOUT)
        : 5000
    });
  }

  public static get<T>(service: { new (): T }): T {
    // TODO: based on config, a mocked service could be generated
    return new service();
  }
}
