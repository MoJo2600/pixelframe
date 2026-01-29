/// <reference types="vite/client" />

interface ImportMetaEnv {
  readonly VITE_API_BASE_URL: string
  readonly VITE_NOTIFICATION_DURATION: string
  readonly VITE_HTTP_CLIENT_TIMEOUT: string
}

interface ImportMeta {
  readonly env: ImportMetaEnv
}
