import { defineStore } from 'pinia'
import { ref, computed } from 'vue'
import { v4 as uuidv4 } from 'uuid'

export const useRendererStore = defineStore('renderer', () => {
  const key = ref<string>(uuidv4())

  const renderKey = computed(() => key.value)

  function forceReRender(): string {
    const newKey = uuidv4()
    key.value = newKey
    return newKey
  }

  return {
    key,
    renderKey,
    forceReRender,
  }
})
