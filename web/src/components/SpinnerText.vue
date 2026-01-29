<template>
  <span class="ml-4 text-overline primary--text">
    {{ text }}
    <v-progress-circular
      class="ml-2"
      :width="2"
      :size="10"
      indeterminate
    ></v-progress-circular>
  </span>
</template>

<script setup lang="ts">
defineProps<{
  text: string
}>()
</script>
