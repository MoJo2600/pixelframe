<template>
  <v-app>
    <v-app-bar app flat>
      <v-app-bar-nav-icon
        class="hidden-lg-and-up"
        @click="drawerVisible = !drawerVisible"
      ></v-app-bar-nav-icon>
    </v-app-bar>

    <navigation v-model="drawerVisible" />

    <v-main :style="{ background }">
      <router-view :key="rendererStore.renderKey"></router-view>
    </v-main>

    <v-container id="notification-container">
      <notification
        v-for="notification in notificationStore.allNotifications"
        :key="notification.id"
        :notification="notification"
      />
    </v-container>
  </v-app>
</template>

<script setup lang="ts">
import { ref, computed, onMounted } from 'vue'
import { useTheme } from 'vuetify'
import Navigation from '@/components/Navigation.vue'
import Notification from '@/components/Notification.vue'
import { useNotificationStore } from '@/store/modules/notification'
import { useRendererStore } from '@/store/modules/renderer'
import { useThemeStore } from '@/store/modules/theme'

const theme = useTheme()
const notificationStore = useNotificationStore()
const rendererStore = useRendererStore()
const themeStore = useThemeStore()

const drawerVisible = ref(true)

const background = computed(() => {
  const currentTheme = theme.global.name.value
  return theme.themes.value[currentTheme]?.colors?.background
})

onMounted(() => {
  theme.change(themeStore.darkThemeEnabled ? 'dark' : 'light')
})
</script>

<style lang="scss">
.skeleton-child-full-width {
  & > div {
    width: 100%;
  }
}

#notification-container {
  width: auto;
  position: fixed;
  top: 64px;
  right: 0;
  z-index: 2000;
  max-width: 480px;
}
</style>
