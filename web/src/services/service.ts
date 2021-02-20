import Axios, { AxiosInstance } from "axios";

export abstract class Service {
  protected readonly baseUrl: string;
  protected readonly httpClient: AxiosInstance;

  constructor() {
    if (!process.env.VUE_APP_API_BASE_URL) {
      throw new Error(
        '"VUE_APP_API_BASE_URL" must be provided in the environment'
      );
    }

    this.baseUrl = process.env.VUE_APP_API_BASE_URL;

    this.httpClient = Axios.create({
      baseURL: this.baseUrl
    });
  }

  public static get<T>(service: { new (): T }): T {
    // TODO: based on config, a mocked service could be generated
    return new service();
  }
}
