<template>
  <v-row>
    <v-col :cols="display.xs.value ? 12 : 4">
      <p class="text-body-1">{{ title }}</p>
    </v-col>
    <v-col :cols="display.xs.value ? 12 : 8">
      <v-skeleton-loader
        v-if="skeletonType === 'input' && (loading || error)"
        :boilerplate="error"
        class="skeleton-child-full-width"
        type="button"
      ></v-skeleton-loader>

      <v-skeleton-loader
        v-if="skeletonType === 'slider' && (loading || error)"
        :boilerplate="error"
        class="mx-auto mt-2"
        type="text"
      ></v-skeleton-loader>

      <slot></slot>
    </v-col>
  </v-row>
</template>

<script setup lang="ts">
import { useDisplay } from 'vuetify'

defineProps<{
  loading: boolean
  error: boolean
  title: string
  skeletonType: 'input' | 'slider'
}>()

const display = useDisplay()
</script>
