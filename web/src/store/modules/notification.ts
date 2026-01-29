import { defineStore } from 'pinia'
import { ref, computed } from 'vue'
import { v4 as uuidv4 } from 'uuid'

export enum NotificationType {
  Success = 'success',
  Information = 'info',
  Warning = 'warning',
  Error = 'error',
}

export interface NotificationData {
  id: string
  dateTime: Date
  type: NotificationType
  content: string
  details?: string
}

export const useNotificationStore = defineStore('notification', () => {
  const notifications = ref<NotificationData[]>([])

  const allNotifications = computed(() => notifications.value)

  // TODO: provide getter for visible notifications only to clean up DOM

  function notify(options: {
    type: NotificationType
    content: string
    details?: string
  }): NotificationData {
    const notification: NotificationData = {
      id: uuidv4(),
      dateTime: new Date(),
      ...options,
    }
    notifications.value.push(notification)
    return notification
  }

  return {
    notifications,
    allNotifications,
    notify,
  }
})
