import Axios, { AxiosInstance } from "axios";

const MAX_REQUESTS_COUNT = 10;
const INTERVAL_MS = 10;
let PENDING_REQUESTS = 0;

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

    /**
     * Axios Request Interceptor
     */
    this.httpClient.interceptors.request.use(function(config) {
      return new Promise(resolve => {
        const interval = setInterval(() => {
          if (PENDING_REQUESTS < MAX_REQUESTS_COUNT) {
            PENDING_REQUESTS++;
            clearInterval(interval);
            resolve(config);
          }
        }, INTERVAL_MS);
      })
    })
    /**
     * Axios Response Interceptor
     */
    this.httpClient.interceptors.response.use(
      function(response) {
        PENDING_REQUESTS = Math.max(0, PENDING_REQUESTS - 1);
        return Promise.resolve(response);
      },
      function(error) {
        PENDING_REQUESTS = Math.max(0, PENDING_REQUESTS - 1);
        return Promise.reject(error);
      }
    );
  }

  public static get<T>(service: { new(): T }): T {
    // TODO: based on config, a mocked service could be generated
    return new service();
  }
}
