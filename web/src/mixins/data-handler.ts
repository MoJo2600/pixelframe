import { ref } from 'vue'
import {
  useNotificationStore,
  NotificationType,
} from '@/store/modules/notification'

export enum WriteAction {
  Create = 'create',
  Update = 'update',
  Delete = 'delete',
  Command = 'command',
}

export function useDataHandler() {
  const notificationStore = useNotificationStore()

  // data loading
  const loading = ref(true)
  const error = ref(false)
  // data sending / updating / deleting
  const writing = ref<string | null>(null)

  async function wrapDataRead(fn: () => Promise<void>): Promise<void> {
    try {
      await fn()
    } catch (err: any) {
      error.value = true

      notificationStore.notify({
        type: NotificationType.Error,
        content: 'Failed to load data',
        details: err.message,
      })
    } finally {
      loading.value = false
    }
  }

  async function wrapDataWrite(
    fn: () => Promise<void>,
    action: WriteAction,
    item: string
  ): Promise<void> {
    try {
      writing.value = item

      await fn()

      notificationStore.notify({
        type: NotificationType.Success,
        content:
          action === WriteAction.Command
            ? `Successfully sent command '${item}'`
            : `Successfully ${action}d ${item}`,
      })
    } catch (err: any) {
      notificationStore.notify({
        type: NotificationType.Error,
        content:
          action === WriteAction.Command
            ? `Failed to send command '${item}'`
            : `Failed to ${action} ${item}`,
        details: err.message,
      })
    } finally {
      writing.value = null
    }
  }

  return {
    loading,
    error,
    writing,
    wrapDataRead,
    wrapDataWrite,
  }
}
