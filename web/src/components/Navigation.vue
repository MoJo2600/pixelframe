<template>
  <v-navigation-drawer v-model="drawerVisible" app hide-overlay>
    <v-list nav>
      <template v-for="item in navRoutes" :key="String(item.name)">
        <v-list-item v-if="!item.children" :to="item.path">
          <template #prepend>
            <v-icon>{{ item.meta?.icon }}</v-icon>
          </template>
          <v-list-item-title>{{ item.name }}</v-list-item-title>
        </v-list-item>

        <v-list-group v-else :value="item.path">
          <template #activator="{ props }">
            <v-list-item v-bind="props" :prepend-icon="String(item.meta?.icon)">
              <v-list-item-title>{{ item.name }}</v-list-item-title>
            </v-list-item>
          </template>

          <v-list-item
            v-for="nestedItem in item.children"
            :key="String(nestedItem.name)"
            :to="`${item.path}/${nestedItem.path}`"
            density="compact"
          >
            <v-list-item-title class="ml-2">{{
              nestedItem.name
            }}</v-list-item-title>
          </v-list-item>
        </v-list-group>
      </template>

      <v-list-item href="/update">
        <template #prepend>
          <v-icon>mdi-memory</v-icon>
        </template>
        <v-list-item-title>Update firmware</v-list-item-title>
      </v-list-item>

      <v-list-item @click="handleSwitchTheme()">
        <template #prepend>
          <v-icon>mdi-theme-light-dark</v-icon>
        </template>
        <v-list-item-title>Theme</v-list-item-title>
      </v-list-item>

      <v-list-item @click="shutdown">
        <template #prepend>
          <v-icon>mdi-power</v-icon>
        </template>
        <v-list-item-title>Off</v-list-item-title>
      </v-list-item>
    </v-list>
  </v-navigation-drawer>
</template>

<script setup lang="ts">
import { computed, watch } from 'vue'
import { useRouter } from 'vue-router'
import { useTheme } from 'vuetify'
import { FramesService, Service } from '@/services'
import { useThemeStore } from '@/store/modules/theme'

const drawerVisible = defineModel<boolean>()

const router = useRouter()
const vuetifyTheme = useTheme()
const themeStore = useThemeStore()
const framesService = Service.get(FramesService)

const navRoutes = computed(() => {
  return (router.options.routes || []).filter(r => r.path !== '/')
})

async function shutdown(): Promise<void> {
  await framesService.showFrame('off')
}

function handleSwitchTheme(): void {
  themeStore.switchTheme()
}

watch(
  () => themeStore.darkThemeEnabled,
  isDark => {
    vuetifyTheme.change(isDark ? 'dark' : 'light')
  }
)
</script>

<style lang="scss" scoped>
.v-list-item--active {
  color: var(--v-primary-base) !important;
}

.v-list-item--active::before {
  background: var(--v-primary-base);
}
</style>
