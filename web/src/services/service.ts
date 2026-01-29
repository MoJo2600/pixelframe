import Axios, { AxiosInstance } from 'axios'

export abstract class Service {
  protected readonly baseUrl: string
  protected readonly httpClient: AxiosInstance

  constructor() {
    this.baseUrl =
      (import.meta.env.VITE_API_BASE_URL as string) || window.location.origin

    console.log(`API: ${this.baseUrl}`)

    this.httpClient = Axios.create({
      baseURL: `${this.baseUrl}/api`,
      timeout: import.meta.env.VITE_HTTP_CLIENT_TIMEOUT
        ? Number(import.meta.env.VITE_HTTP_CLIENT_TIMEOUT)
        : 5000,
    })
  }

  public static get<T>(service: { new (): T }): T {
    // TODO: based on config, a mocked service could be generated
    return new service()
  }
}
