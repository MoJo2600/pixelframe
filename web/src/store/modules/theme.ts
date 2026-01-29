import { defineStore } from 'pinia'
import { ref, computed } from 'vue'

export const useThemeStore = defineStore('theme', () => {
  const darkTheme = ref(false)

  const darkThemeEnabled = computed(() => darkTheme.value)

  function switchTheme(): boolean {
    darkTheme.value = !darkTheme.value
    // TODO: localStorage.put
    return darkTheme.value
  }

  function setDarkTheme(value: boolean): void {
    darkTheme.value = value
    // TODO: localStorage.put
  }

  return {
    darkTheme,
    darkThemeEnabled,
    switchTheme,
    setDarkTheme,
  }
})
