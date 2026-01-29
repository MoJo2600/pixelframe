<template>
  <v-alert
    v-model="visible"
    closable
    :type="notification.type"
    variant="elevated"
    transition="scale-transition"
    class="notification-alert"
  >
    <p class="text-body-1 mb-0">{{ notification.content }}</p>
    <span v-if="notification.details" class="text-caption">{{
      notification.details
    }}</span>
  </v-alert>
</template>

<script setup lang="ts">
import { ref, onMounted } from 'vue'
import type { NotificationData } from '@/store/modules/notification'

defineProps<{
  notification: NotificationData
}>()

const notificationVisibleDuration = import.meta.env.VITE_NOTIFICATION_DURATION
  ? Number(import.meta.env.VITE_NOTIFICATION_DURATION)
  : 4000

const visible = ref(true)

onMounted(() => {
  setTimeout(() => {
    visible.value = false
  }, notificationVisibleDuration)
})
</script>

<style lang="scss" scoped>
.v-alert__content {
  margin: 0;
}

.notification-alert {
  :deep(.v-alert__close) {
    color: inherit;
    opacity: 0.9;

    .v-btn {
      color: inherit;
    }
  }
}
</style>
